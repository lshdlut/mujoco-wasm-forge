"""Regression checks for generated field widths and dependency preservation."""
import tempfile
import unittest
import os
import subprocess
import sys
import json
import hashlib
from argparse import Namespace
from pathlib import Path
from unittest.mock import patch, DEFAULT

import forge_cli
from abi_exports import gen_structs as structs
from abi_exports import gen_scene_geom_soa as soa
from abi_exports import gen_funcs as funcs
from abi_exports.signature_caps import generate_capabilities, SignaturePolicy


class GeneratorTypes(unittest.TestCase):
    def test_non_ninja_cache_is_refused_without_modification(self):
        with tempfile.TemporaryDirectory() as directory:
            build = Path(directory)
            cache = build / 'CMakeCache.txt'
            original = b'CMAKE_GENERATOR:INTERNAL=Unix Makefiles\n'
            cache.write_bytes(original)
            with patch.object(forge_cli.subprocess, 'run') as run:
                with self.assertRaisesRegex(SystemExit, 'cache is preserved'):
                    forge_cli._configure_wasm_build_dir('3.15.0', build, {}, False)
                run.assert_not_called()
            self.assertEqual(cache.read_bytes(), original)

    def test_ninja_resolution_requires_explicit_reproducible_generator(self):
        with patch.object(forge_cli.os, 'name', 'posix'), patch.object(forge_cli.shutil, 'which', return_value='/usr/bin/ninja'):
            self.assertEqual(forge_cli._resolve_ninja_executable({'PATH': '/usr/bin'}), '/usr/bin/ninja')
        with tempfile.TemporaryDirectory() as directory:
            executable = Path(directory) / 'ninja'; executable.write_bytes(b'fixture')
            self.assertEqual(forge_cli._resolve_ninja_executable({'MJWF_NINJA': str(executable)}), str(executable))
            with self.assertRaisesRegex(SystemExit, 'Invalid MJWF_NINJA'):
                forge_cli._resolve_ninja_executable({'MJWF_NINJA': str(executable) + '-missing'})
        with patch.object(forge_cli.os, 'name', 'posix'), patch.object(forge_cli.shutil, 'which', return_value=None):
            with self.assertRaisesRegex(SystemExit, 'Ninja is required'):
                forge_cli._resolve_ninja_executable({'PATH': ''})

    def test_selector_ignores_only_exact_namespace_migration(self):
        records = '\n'.join((
            'R100\tdist/3.8.1/mujoco.wasm\tdeliverables/3.8.1/mujoco.wasm',
            'R099\tdist/3.8.1/mujoco.js\tdeliverables/3.8.1/mujoco.js',
            'R100\tdist/3.8.1/mujoco.js\tdeliverables/3.9.0/mujoco.js',
            'A\tdeliverables/3.15.0/mujoco.wasm',
            'M\tdeliverables/3.14.0/mujoco.js',
            'M\tabi_exports/gen_funcs.py',
        ))
        selected = forge_cli._paths_for_build_selection(records)
        self.assertNotIn('deliverables/3.8.1/mujoco.wasm', selected)
        self.assertIn('deliverables/3.8.1/mujoco.js', selected)
        self.assertIn('deliverables/3.9.0/mujoco.js', selected)
        self.assertIn('deliverables/3.15.0/mujoco.wasm', selected)
        self.assertIn('deliverables/3.14.0/mujoco.js', selected)
        self.assertIn('abi_exports/gen_funcs.py', selected)
        self.assertEqual(forge_cli._paths_for_build_selection('M\tforge_cli.py\n'), ['forge_cli.py'])

    def test_literal_variadic_messages(self):
        source = funcs.generate_source([funcs.FunctionDecl(name, 'void', ['const char* msg'], ['msg'])
                                        for name in ('mju_warning', 'mju_error')])
        self.assertIn('mju_warning("%s", msg)', source)
        self.assertIn('mju_error("%s", msg)', source)
        info = funcs.generate_source([funcs.FunctionDecl('mju_info', 'void',
                                      ['int topic', 'const char* msg'], ['topic', 'msg'])])
        self.assertIn('mju_info(topic, "%s", msg)', info)

    def test_scalar_widths_and_dimensions(self):
        fields = [structs.FieldInfo("mjModel", name, {"kind": "ValueType", "name": base}, None, None)
                  for name, base in (("nq", "int"), ("nbuffer", "mjtSize"),
                                     ("stamp", "uint64_t"), ("flag", "mjtBool"))]
        dims = structs.collect_dim_exports({"mjModel": fields})
        self.assertEqual({d.base_name: d.ctype for d in dims}, {"model_nq": "int", "model_nbuffer": "int", "model_nbuffer_i64": "int64_t"})
        self.assertIn("return (int64_t)(m->nbuffer)", structs.generate_dim_impl(dims[2]))
        pointers = structs.collect_pointer_exports({"mjModel": fields})
        self.assertEqual({p.field: p.ctype for p in pointers}, {"stamp": "uint64_t*", "flag": "uint8_t*"})
        self.assertEqual(soa.AUTO_DTYPE_BY_BASE["mjtBool"], "u8")

    def test_dirty_dependency_is_preserved(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            dependency = root / "external" / "mujoco"
            (dependency / ".git").mkdir(parents=True)
            result = subprocess.CompletedProcess([], 0, " M CMakeLists.txt\n", "")
            with patch.object(forge_cli, "REPO_ROOT", root), patch.object(forge_cli.subprocess, "run", return_value=result) as run:
                with self.assertRaisesRegex(SystemExit, "Refusing to overwrite dirty"):
                    forge_cli._prepare_mujoco("3.15.0", False)
                self.assertEqual(run.call_count, 1)
                self.assertIn("status", run.call_args.args[0])

    def test_fresh_dependency_preserves_checkout_on_success_and_failure(self):
        work = Path(__file__).resolve().parents[2] / 'build' / 'type-tests'
        work.mkdir(parents=True, exist_ok=True)
        for fail_clone in (False, True):
            with self.subTest(fail_clone=fail_clone), tempfile.TemporaryDirectory(dir=work) as directory:
                root = Path(directory)
                dependency = root / 'external' / 'mujoco'
                (dependency / '.git').mkdir(parents=True)
                (dependency / 'private.bin').write_bytes(b'unchanged ignored runtime')
                def run(argv, **kwargs):
                    if 'clone' in argv:
                        if fail_clone:
                            raise subprocess.CalledProcessError(1, argv)
                        (dependency / '.git').mkdir(parents=True)
                    return subprocess.CompletedProcess(argv, 0, '', '')
                with patch.object(forge_cli, 'REPO_ROOT', root), patch.object(forge_cli.subprocess, 'run', side_effect=run), patch.multiple(
                        forge_cli, _patch_mujoco_qhull_emscripten=DEFAULT,
                        _patch_mujoco_localtime_emscripten=DEFAULT,
                        _patch_mujoco_disable_default_compiler_threads_emscripten=DEFAULT):
                    if fail_clone:
                        with self.assertRaises(subprocess.CalledProcessError):
                            forge_cli._prepare_mujoco('3.15.0', False, fresh_dependency=True)
                    else:
                        forge_cli._prepare_mujoco('3.15.0', False, fresh_dependency=True)
                snapshots = list((root / 'build' / 'dependency-snapshots').glob('mujoco-*'))
                self.assertEqual(len(snapshots), 1)
                self.assertEqual((snapshots[0] / 'private.bin').read_bytes(), b'unchanged ignored runtime')
                self.assertEqual(dependency.exists(), not fail_clone)

    def test_verify_dist_rejects_failed_audit_without_success_coercion(self):
        import json
        work = Path(__file__).resolve().parents[2] / 'build' / 'type-tests'
        work.mkdir(parents=True, exist_ok=True)
        for failed in (None, 'deliverables', 'ci-build/dist'):
            with self.subTest(failed=failed), tempfile.TemporaryDirectory(dir=work) as directory:
                root = Path(directory)
                for tree in ('deliverables', 'ci-build/dist'):
                    abi = root / tree / '3.15.0' / 'abi'
                    abi.mkdir(parents=True)
                    (abi.parent / 'mujoco.wasm').write_bytes(b'fixture-wasm')
                    data = dict(schemaVersion=2, variant='single', wasmSha256=hashlib.sha256(b'fixture-wasm').hexdigest(),
                                ok=tree != failed, error='intentional nm failure' if tree == failed else '',
                                artifact=tree, nmPath=tree, count=1, symbols=['mj_version'])
                    (abi / 'nm_coverage.json').write_text(json.dumps(data))
                    (abi / 'nm_coverage.single.json').write_text(json.dumps(data))
                args = Namespace(version=['3.15.0'], ci_build_dir='ci-build')
                with patch.object(forge_cli, 'REPO_ROOT', root), patch.object(forge_cli, 'DELIVERABLES_ROOT', root / 'deliverables'):
                    if failed:
                        with self.assertRaisesRegex(SystemExit, 'intentional nm failure'):
                            forge_cli.cmd_verify_dist(args)
                        report = json.loads((root / failed / '3.15.0/abi/nm_coverage.json').read_text())
                        self.assertFalse(report['ok'])
                        self.assertEqual(report['error'], 'intentional nm failure')
                    else:
                        with patch.object(forge_cli.subprocess, 'run', side_effect=FileNotFoundError):
                            self.assertEqual(forge_cli.cmd_verify_dist(args), 0)

    def test_missing_stale_and_wrong_flavor_nm_reports_are_rejected(self):
        work = Path(__file__).resolve().parents[2] / 'build' / 'type-tests'
        work.mkdir(parents=True, exist_ok=True)
        for defect in ('missing', 'stale', 'wrong-flavor', 'stale-alias'):
            with self.subTest(defect=defect), tempfile.TemporaryDirectory(dir=work) as directory:
                root = Path(directory)
                abi = root / 'abi'; abi.mkdir()
                (root / 'mujoco.wasm').write_bytes(b'fixture')
                data = dict(schemaVersion=2, variant='single', ok=True, error=None,
                            wasmSha256=hashlib.sha256(b'fixture').hexdigest())
                if defect == 'stale': data['wasmSha256'] = '0' * 64
                if defect == 'wrong-flavor': data['variant'] = 'pthreads'
                if defect != 'missing': (abi / 'nm_coverage.single.json').write_text(json.dumps(data))
                shared = dict(data)
                if defect == 'stale-alias': shared['wasmSha256'] = '1' * 64
                (abi / 'nm_coverage.json').write_text(json.dumps(shared))
                with self.assertRaises(SystemExit): forge_cli._validate_nm_reports(root)

    def test_nm_process_failure_is_nonzero(self):
        # The SDK default is only local fallback; CI already exports EMSDK/PATH.
        node = forge_cli._resolve_node_executable(dict(os.environ, EMSDK=os.environ.get('EMSDK', 'C:/emsdk')))
        root = Path(__file__).resolve().parents[2]
        result = subprocess.run([node, str(root / 'abi_impl/nm_coverage.mjs'), str(root / 'build/missing-audit-fixture.a')],
                                capture_output=True, text=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertFalse(json.loads(result.stdout)['ok'])

    def test_post_build_missing_archive_is_fatal(self):
        root = Path(__file__).resolve().parents[2]
        work = root / 'build' / 'type-tests'; work.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=work) as directory:
            fixture = Path(directory)
            dist = fixture / 'dist' / 'postfixture'; dist.mkdir(parents=True)
            (dist / 'mujoco.js').write_text('// fixture')
            (dist / 'mujoco.wasm').write_bytes(b'fixture')
            (fixture / 'check').mkdir()
            (fixture / 'check/check_exports.mjs').write_text('process.exit(0);')
            env = dict(os.environ, NODE=forge_cli._resolve_node_executable(dict(os.environ, EMSDK=os.environ.get('EMSDK', 'C:/emsdk'))),
                       MJWF_BUILD_ROOT=str(fixture / 'build').replace('\\','/'))
            result = subprocess.run(forge_cli._bash_argv(str(root / 'check/post_build.sh').replace('\\','/'),
                                    '--version','postfixture','--short','99999'), cwd=fixture, env=env,
                                    capture_output=True, text=True)
            self.assertNotEqual(result.returncode,0)
            self.assertIn('error: libmujoco archive not found',result.stderr)

    def test_build_metadata_records_actual_compiler_and_patched_inputs(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            compiler = root / 'sdk' / 'emscripten'; compiler.mkdir(parents=True)
            (compiler / 'emscripten-version.txt').write_text('"4.0.10"')
            build = root / 'build'; build.mkdir()
            (build / 'CMakeCache.txt').write_text('CMAKE_C_COMPILER:FILEPATH=' + str(compiler / 'emcc') + '\n')
            with patch.object(forge_cli, 'REPO_ROOT', root):
                prefix_flags = forge_cli._source_prefix_flags()
            cache_text = (build / 'CMakeCache.txt').read_text()
            valid_cache = cache_text + 'CMAKE_C_FLAGS:STRING=' + prefix_flags + '\nCMAKE_CXX_FLAGS:STRING=' + prefix_flags + '\nCMAKE_GENERATOR:INTERNAL=Ninja\n'
            (build / 'CMakeCache.txt').write_text(valid_cache)
            dist = root / 'dist' / '3.15.0'; (dist / 'abi').mkdir(parents=True)
            (dist / 'mujoco.wasm').write_bytes(b'single')
            (dist / 'pthreads').mkdir(); (dist / 'pthreads' / 'mujoco.wasm').write_bytes(b'threads')
            patch_text = 'diff --git a/source.c b/source.c\n+intentional patch\n'
            with patch.object(forge_cli, 'REPO_ROOT', root), patch.object(forge_cli.subprocess, 'check_output',
                    side_effect=['a' * 40, patch_text, ' M source.c\n'] * 2):
                forge_cli._write_build_metadata('3.15.0', build, '')
                forge_cli._write_build_metadata('3.15.0', build, 'pthreads')
            metadata = json.loads((dist / 'abi' / 'build_metadata.json').read_text())
            self.assertEqual(metadata['emsdkVersion'], '4.0.10')
            self.assertEqual(set(metadata['flavors']), {'single', 'pthreads'})
            self.assertTrue(metadata['flavors']['single']['upstreamDirty'])
            self.assertEqual(metadata['flavors']['single']['settings']['cmakeGenerator'], 'Ninja')
            self.assertEqual(metadata['flavors']['single']['upstreamPatchSha256'], hashlib.sha256(patch_text.encode()).hexdigest())
            self.assertNotIn(str(root), json.dumps(metadata))
            (build / 'CMakeCache.txt').write_text(valid_cache.replace('CMAKE_GENERATOR:INTERNAL=Ninja', 'CMAKE_GENERATOR:INTERNAL=Unix Makefiles'))
            with patch.object(forge_cli, 'REPO_ROOT', root), patch.object(forge_cli.subprocess, 'check_output',
                    side_effect=['a' * 40, patch_text, ' M source.c\n']):
                with self.assertRaisesRegex(SystemExit, 'actual Ninja generator'):
                    forge_cli._write_build_metadata('3.15.0', build, '')
            (build / 'CMakeCache.txt').write_text(cache_text)
            with patch.object(forge_cli, 'REPO_ROOT', root), patch.object(forge_cli.subprocess, 'check_output',
                    side_effect=['a' * 40, patch_text, ' M source.c\n']):
                with self.assertRaisesRegex(SystemExit, r'actual configured C/C\+\+ source prefix maps'):
                    forge_cli._write_build_metadata('3.15.0', build, '')
            (build / 'CMakeCache.txt').write_text('')
            with patch.object(forge_cli, 'REPO_ROOT', root), patch.object(forge_cli.subprocess, 'check_output',
                    side_effect=['a' * 40, patch_text, ' M source.c\n']):
                with self.assertRaisesRegex(SystemExit, 'actual configured C compiler'):
                    forge_cli._write_build_metadata('3.15.0', build, '')

    def test_signature_policy_is_conservative_and_type_driven(self):
        value = lambda name: dict(kind='ValueType', name=name)
        record = dict(fields=[dict(name='x', type=value('int')), dict(name='y', type=value('double'))])
        unsafe = dict(fields=[dict(name='callback', type=value('mjfHook'))])
        policy = SignaturePolicy({'POD': record, 'Unsafe': unsafe}, {})
        self.assertTrue(policy.pod('POD')); self.assertFalse(policy.pod('Unsafe'))
        self.assertEqual(policy.classify(value('uint64_t'))['resultNormalization'], 'BigInt.asUintN(64, rawResult)')
        self.assertEqual(policy.classify(value('mjfHook'))['kind'], 'callback')
        self.assertEqual(policy.classify(value('UnknownAlias'))['kind'], 'unknown')
        f = funcs.FunctionDecl('arbitrary_name', 'POD', ['POD input'], ['input'], value('POD'), [value('POD')])
        caps, adapters = generate_capabilities([f], {'POD': record}, {}, funcs.FunctionDecl)
        self.assertEqual(caps['functions'][f.name]['adapter']['symbol'], 'mjwf_arbitrary_name_ptr_out')
        source = funcs.generate_source(adapters)
        self.assertIn('*mjwf_out = arbitrary_name(*input)', source)
        self.assertIn('offsetof(POD, x)', source)

    def test_file_prefix_map_reproducible_in_wasm(self):
        sdk = Path(os.environ.get('EMSDK', 'C:/emsdk'))
        work = Path(__file__).resolve().parents[2] / 'build' / 'type-tests'; work.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=work) as directory:
            root = Path(directory)
            binaries = []
            unmapped_binaries = []
            for name in ('a', 'different-longer-checkout'):
                folder = root / name; folder.mkdir()
                source = folder / 'probe.c'; source.write_text('const char* path(void) {return __FILE__;}\n')
                env = dict(os.environ, EM_CONFIG=str(sdk / '.emscripten'))
                command = [sys.executable, str(sdk / 'upstream/emscripten/emcc.py'), source.as_posix(),
                                '-ffile-prefix-map=' + folder.as_posix() + '=.', '--no-entry', '-O3',
                                '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=node',
                                '-sEXPORTED_FUNCTIONS=["_path"]', '-sEXPORTED_RUNTIME_METHODS=["UTF8ToString"]',
                           '-o', str(folder / 'probe.mjs')]
                subprocess.run(command, env=env, check=True)
                binaries.append((folder / 'probe.wasm').read_bytes())
                unmapped = [arg for arg in command if not arg.startswith('-ffile-prefix-map=')]
                unmapped[-1] = str(folder / 'unmapped.mjs')
                subprocess.run(unmapped, env=env, check=True)
                unmapped_binaries.append((folder / 'unmapped.wasm').read_bytes())
                runner = folder / 'runner.mjs'
                runner.write_text("import assert from 'node:assert/strict'; import fs from 'node:fs'; import factory from './probe.mjs';\n"
                                  "const m=await factory({wasmBinary:fs.readFileSync(new URL('./probe.wasm',import.meta.url))});\n"
                                  "assert.equal(m.UTF8ToString(m._path()),'probe.c');\n")
                subprocess.run([forge_cli._resolve_node_executable(dict(os.environ, EMSDK=str(sdk))), str(runner)], check=True)
            self.assertEqual(binaries[0], binaries[1])
            self.assertNotEqual(unmapped_binaries[0], unmapped_binaries[1])

    def test_checked_narrowing_in_wasm(self):
        sdk = Path(os.environ.get("EMSDK", "C:/emsdk"))
        work = Path(__file__).resolve().parents[2] / "build" / "type-tests"
        work.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=work) as directory:
            root = Path(directory)
            source = root / "probe.c"
            source.write_text("#include <stdint.h>\n#include <limits.h>\n#include <stdio.h>\n" +
                              structs.DIMENSION_HELPER +
                              '\nint check(int64_t value) { return mjwf_dimension_i32(value, "fixture"); }\n')
            value = lambda name: dict(kind='ValueType', name=name)
            fixture_funcs = [
                funcs.FunctionDecl('fixture_i64', 'int64_t', ['int64_t value'], ['value'], value('int64_t'), [value('int64_t')]),
                funcs.FunctionDecl('fixture_u64', 'uint64_t', ['uint64_t value'], ['value'], value('uint64_t'), [value('uint64_t')]),
                funcs.FunctionDecl('fixture_return', 'POD', [], [], value('POD'), []),
                funcs.FunctionDecl('fixture_input', 'int', ['POD p'], ['p'], value('int'), [value('POD')]),
            ]
            _, generated = generate_capabilities(fixture_funcs, {'POD': dict(fields=[dict(name='x', type=value('int')),
                                                        dict(name='y', type=value('double'))])}, {}, funcs.FunctionDecl)
            fixture = '\ntypedef struct {int x; double y;} POD;\nint64_t fixture_i64(int64_t x) {return x*2;}\nuint64_t fixture_u64(uint64_t x) {return x;}\nPOD fixture_return(void) {return (POD){7,3.5};}\nint fixture_input(POD p) {return p.x;}\n'
            code = funcs.generate_source([*fixture_funcs, *generated]).replace('#include "mjwf_abi_funcs.h"', '')
            source.write_text(source.read_text() + fixture + code)
            env = dict(os.environ, EM_CONFIG=str(sdk / ".emscripten"))
            subprocess.run([sys.executable, str(sdk / "upstream" / "emscripten" / "emcc.py"),
                            str(source), '--no-entry', '-sEXPORTED_FUNCTIONS=' + json.dumps(['_check', '_malloc', '_free'] +
                                                      ['_mjwf_' + f.name for f in [*fixture_funcs, *generated]]),
                            '-sWASM_BIGINT=1', '-sMODULARIZE=1', '-sEXPORT_ES6=1',
                            '-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]',
                            '-sENVIRONMENT=node', '-o', str(root / 'probe.mjs')], env=env, check=True)
            runner = root / "runner.mjs"
            runner.write_text("import assert from 'node:assert/strict';\nimport fs from 'node:fs';\n"
                              "import factory from './probe.mjs';\n"
                              "const m = await factory({wasmBinary: fs.readFileSync(new URL('./probe.wasm', import.meta.url))});\n"
                              "assert.equal(m._check(2147483647n),2147483647);\n"
                              "assert.equal(m._check(-2147483648n),-2147483648);\n"
                              "assert.equal(m._check(2147483648n),-1);\n"
                              "assert.equal(m._check(-2147483649n),-1);\n")
            runner.write_text(runner.read_text() +
                             "assert.equal(m._mjwf_fixture_i64(1n<<40n),2n<<40n);\n" +
                             "assert.throws(()=>m._mjwf_fixture_i64(5),TypeError);\n" +
                             "const u=(1n<<63n)+7n; assert.equal(BigInt.asUintN(64,m._mjwf_fixture_u64(u)),u);\n" +
                             "const ptr=m._malloc(m._mjwf_sizeof_POD()); m._mjwf_fixture_return_out(ptr);\n" +
                             "const view=new DataView(m.HEAPU8.buffer); assert.equal(view.getInt32(ptr+m._mjwf_offsetof_POD_x(),true),7);\n" +
                             "assert.equal(view.getFloat64(ptr+m._mjwf_offsetof_POD_y(),true),3.5);\n" +
                             "assert.equal(m._mjwf_fixture_input_ptr(ptr),7); m._free(ptr);\n")
            node = forge_cli._resolve_node_executable(dict(os.environ, EMSDK=str(sdk)))
            result = subprocess.run([node, str(runner)], check=True, capture_output=True, text=True)
            self.assertEqual(result.stderr.count("exceeds i32 range"), 2)


if __name__ == "__main__":
    forge_cli._assert_external_runtime_checkout()
    unittest.main()

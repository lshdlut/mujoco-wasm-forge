from __future__ import annotations

import json
import subprocess
import sys
import tempfile
import unittest
import zipfile
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_PATH = REPO_ROOT / "tools" / "package_release_assets.py"


class PackageReleaseContractTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tempdir = tempfile.TemporaryDirectory()
        self.root = Path(self.tempdir.name)
        self.dist_ver = self.root / "deliverables" / "3.15.0"
        (self.dist_ver / "abi").mkdir(parents=True)
        (self.dist_ver / "pthreads").mkdir()
        (self.dist_ver / "mujoco.js").write_text("single-js\n", encoding="utf-8")
        (self.dist_ver / "mujoco.wasm").write_bytes(b"single-wasm")
        (self.dist_ver / "pthreads" / "mujoco.js").write_text("pthread-js\n", encoding="utf-8")
        (self.dist_ver / "pthreads" / "mujoco.wasm").write_bytes(b"pthread-wasm")
        (self.dist_ver / "pthreads" / "mjwasm_forge.js").write_text("pthread-js\n", encoding="utf-8")
        (self.dist_ver / "pthreads" / "mjwasm_forge.wasm").write_bytes(b"pthread-wasm")
        (self.dist_ver / "abi" / "exports.lst").write_text("_mj_version\n", encoding="utf-8")
        (self.dist_ver / "abi" / "js_signature_capabilities.json").write_text("{}\n", encoding="utf-8")
        (self.dist_ver / "abi" / "nm_coverage.single.json").write_text('{"flavor": "single"}\n', encoding="utf-8")
        (self.dist_ver / "abi" / "nm_coverage.pthreads.json").write_text('{"flavor": "pthreads"}\n', encoding="utf-8")
        self.legal_dir = self.root / "legal"
        self.legal_dir.mkdir()
        (self.legal_dir / "LICENSE").write_text("Apache License\n", encoding="utf-8")
        (self.legal_dir / "notices").mkdir()
        (self.legal_dir / "notices" / "muJoCo.txt").write_text("Upstream notice\n", encoding="utf-8")

    def tearDown(self) -> None:
        self.tempdir.cleanup()

    def run_packager(self, out_dir: Path) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            [
                sys.executable,
                str(SCRIPT_PATH),
                "--dist-dir",
                str(self.root / "deliverables"),
                "--version",
                "3.15.0",
                "--dist-id",
                "3.15.0-r-local",
                "--git-sha",
                "deadbeef",
                "--out-dir",
                str(out_dir),
                "--legal-dir",
                str(self.legal_dir),
            ],
            cwd=str(REPO_ROOT),
            text=True,
            capture_output=True,
        )

    def test_verified_receipt_is_normalized_without_absolute_paths(self) -> None:
        (self.dist_ver / "abi" / "build_metadata.json").write_text(
            json.dumps(
                {
                    "upstreamResolvedSHA": "a" * 40,
                    "emsdkVersion": "4.0.10",
                    "receiptPath": "C:\\dev\\mujoco-wasm-forge\\workspace\\receipt.json",
                }
            ),
            encoding="utf-8",
        )
        result = self.run_packager(self.root / "out")
        self.assertEqual(result.returncode, 0, result.stderr)

        with zipfile.ZipFile(self.root / "out" / "dist-runtime.zip") as handle:
            names = set(handle.namelist())
            self.assertNotIn("abi/exports.lst", names)
            self.assertIn("LICENSE", names)
            self.assertIn("notices/muJoCo.txt", names)
            manifest = json.loads(handle.read("provenance.json"))
            self.assertEqual(manifest["provenanceStatus"], "verified")
            self.assertEqual(manifest["upstreamResolvedSHA"], "a" * 40)
            self.assertEqual(manifest["sdk"], "4.0.10")
            self.assertEqual(manifest["metadataSource"], "abi/build_metadata.json")
            self.assertEqual(manifest["abiArtifacts"]["nmCoverage"]["single"]["path"], "abi/nm_coverage.single.json")
            self.assertEqual(
                manifest["abiArtifacts"]["jsSignatureCapabilities"]["path"],
                "abi/js_signature_capabilities.json",
            )
            self.assertTrue(all(":" not in key for key in manifest["runtimeFileSHA"]))
            self.assertTrue(all(not value.startswith("C:\\") for value in manifest.values() if isinstance(value, str)))

        with zipfile.ZipFile(self.root / "out" / "dist-audit.zip") as handle:
            self.assertIn("abi/exports.lst", handle.namelist())
            self.assertIn("provenance.json", handle.namelist())

    def test_incomplete_worker_alias_fails_loudly(self) -> None:
        (self.dist_ver / "pthreads" / "mjwasm_forge.wasm").unlink()
        result = self.run_packager(self.root / "out")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("worker alias", result.stderr + result.stdout)

    def test_malformed_receipt_fails_loudly(self) -> None:
        (self.dist_ver / "abi" / "build_metadata.json").write_text("not-json\n", encoding="utf-8")
        result = self.run_packager(self.root / "out")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("invalid JSON metadata file", result.stderr + result.stdout)


if __name__ == "__main__":
    unittest.main()

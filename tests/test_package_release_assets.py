from __future__ import annotations

import subprocess
import sys
import tempfile
import unittest
import zipfile
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_PATH = REPO_ROOT / "tools" / "package_release_assets.py"


class PackageReleaseAssetsTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tempdir = tempfile.TemporaryDirectory()
        self.root = Path(self.tempdir.name)
        self.dist_ver = self.root / "dist" / "3.5.0"
        (self.dist_ver / "abi").mkdir(parents=True)
        (self.dist_ver / "pthreads").mkdir(parents=True)
        (self.dist_ver / "mujoco.js").write_text("runtime-js\n", encoding="utf-8")
        (self.dist_ver / "mujoco.wasm").write_bytes(b"\0asm")
        (self.dist_ver / "abi" / "exports.lst").write_text("_mj_version\n", encoding="utf-8")
        (self.dist_ver / "abi" / "mujoco_ast.json").write_text("{\"normalized\": true}\n", encoding="utf-8")
        (self.dist_ver / "pthreads" / "mujoco.js").write_text("pthread-js\n", encoding="utf-8")

    def tearDown(self) -> None:
        self.tempdir.cleanup()

    def test_packaging_splits_runtime_and_audit_assets(self) -> None:
        subprocess.run(
            [
                sys.executable,
                str(SCRIPT_PATH),
                "--dist-dir",
                str(self.root / "dist"),
                "--version",
                "3.5.0",
                "--dist-id",
                "3.5.0-r2",
                "--git-sha",
                "0123456789abcdef0123456789abcdef01234567",
                "--out-dir",
                str(self.root / "out"),
            ],
            check=True,
            cwd=str(REPO_ROOT),
        )

        runtime_zip = self.root / "out" / "dist-runtime.zip"
        audit_zip = self.root / "out" / "dist-audit.zip"
        self.assertTrue(runtime_zip.is_file())
        self.assertTrue(audit_zip.is_file())

        with zipfile.ZipFile(runtime_zip) as handle:
            names = set(handle.namelist())
            self.assertIn("mujoco.js", names)
            self.assertIn("mujoco.wasm", names)
            self.assertIn("version.json", names)
            self.assertIn("pthreads/mujoco.js", names)
            self.assertIn("pthreads/version.json", names)
            self.assertNotIn("abi/exports.lst", names)
            payload = handle.read("version.json").decode("utf-8")
            self.assertIn('"assetKind": "runtime"', payload)

        with zipfile.ZipFile(audit_zip) as handle:
            names = set(handle.namelist())
            self.assertIn("abi/exports.lst", names)
            self.assertIn("abi/mujoco_ast.json", names)
            self.assertIn("version.json", names)
            self.assertNotIn("mujoco.js", names)
            payload = handle.read("version.json").decode("utf-8")
            self.assertIn('"assetKind": "audit"', payload)


if __name__ == "__main__":
    unittest.main()

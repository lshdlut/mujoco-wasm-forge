# Add a new MuJoCo version

This page describes the typical workflow for adding a new committed `deliverables/<ver>/` baseline. Builds are staged under `dist/<ver>/` in the dev checkout.

## Steps

1. Pick a target ref (tag or commit hash).
2. Build it:

   ```bash
   python3 forge_cli.py build --version <mjver> --with-checks
   ```

3. Review ABI/export diffs:
   - `dist/<mjver>/abi/exports.lst`
   - `dist/<mjver>/abi/exports_report_funcs.md`
4. If exports changed, decide whether it’s acceptable and update wrappers/gates as needed.
5. Copy the validated `dist/<mjver>/` tree to `deliverables/<mjver>/` in the source checkout, then commit that baseline and any pipeline changes.

## Tips

- Prefer building tags like `3.5.0` unless you need a specific commit.
- Run `verify-dist` in CI when committing a new `deliverables/<ver>` baseline.

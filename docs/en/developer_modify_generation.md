# Modify introspection / wrapper generation

Changes in these areas tend to cascade into `dist/<ver>/abi/*` outputs and may affect exported symbols.

## Touch points

- Header scanning / AST extraction: `introspect/`
- Wrapper and export list generation: `abi_exports/`
- Symbol inventory / coverage: `abi_impl/`
- Build/link flags: `app/CMakeLists.txt`

## Recommended workflow

1. Make your code change.
2. Rebuild a canary version with checks:

   ```bash
   python3 forge_cli.py build --version <mjver> --with-checks
   ```

3. Review `dist/<ver>/abi/*` diffs and ensure changes are intentional.
4. Update or add gates/tests if the contract changes.


"""Conservative wasm32 signature policy, not a JavaScript object/lifecycle SDK.

Use the existing introspection type trees to keep unsupported signatures visible.
Only records containing known scalars, fixed arrays, and other such records get
pointer/out adapters; pointers, callbacks, anonymous fields and unknown types
inside a record deliberately stop automatic POD adaptation.
"""
import re


SCALARS = {
    'void': 'void', 'float': 'f32', 'mjtFloat': 'f32', 'double': 'f64', 'mjtNum': 'f64',
    'int': 'i32', 'unsigned int': 'u32', 'unsigned': 'u32', 'int32_t': 'i32',
    'uint32_t': 'u32', 'size_t': 'u32', 'uintptr_t': 'u32', 'intptr_t': 'i32',
    'char': 'i8', 'signed char': 'i8', 'unsigned char': 'u8', 'uint8_t': 'u8',
    'mjtByte': 'u8', 'mjtBool': 'u8', 'bool': 'u8', '_Bool': 'u8',
    'short': 'i16', 'unsigned short': 'u16', 'int16_t': 'i16', 'uint16_t': 'u16',
    'mjtSize': 'i64', 'int64_t': 'i64', 'uint64_t': 'u64',
    'long long': 'i64', 'unsigned long long': 'u64',
}


class SignaturePolicy:
    def __init__(self, structs, enums):
        self.structs = structs
        self.enums = set(enums)

    def pod(self, name, seen=()):
        record = self.structs.get(name)
        if not record or not record.get('fields') or name in seen:
            return False
        for field in record['fields']:
            if not re.fullmatch(r'[A-Za-z_]\w*', field.get('name', '')):
                return False
            if not self.pod_type(field.get('type', {}), (*seen, name)):
                return False
        return True

    def pod_type(self, node, seen=()):
        kind = node.get('kind')
        if kind == 'ArrayType':
            extents = node.get('extents', [])
            return bool(extents) and all(str(n).isdigit() and int(n) > 0 for n in extents) and self.pod_type(node.get('inner', {}), seen)
        if kind != 'ValueType':
            return False
        name = node.get('name')
        return (name in SCALARS and name != 'void') or name in self.enums or self.pod(name, seen)

    def classify(self, node):
        kind = node.get('kind')
        if kind in ('FunctionType', 'FunctionPointerType'):
            return dict(kind='callback', requirement='function-table-and-lifetime-bridge')
        if kind == 'PointerType':
            inner = self.classify(node.get('inner', {}))
            return dict(kind='pointer', jsType='Number', pointee=inner,
                        requirement='valid-wasm-memory-and-lifetime')
        if kind == 'ArrayType':
            return dict(kind='pointer', jsType='Number', pointee=self.classify(node.get('inner', {})),
                        requirement='valid-wasm-memory-and-lifetime', extents=node.get('extents'))
        if kind != 'ValueType':
            return dict(kind='unknown', requirement='needs-adapter-or-policy-review', type=node)
        name = node.get('name', '')
        if name.startswith('mjf'):
            return dict(kind='callback', cType=name, requirement='function-table-and-lifetime-bridge')
        if name in SCALARS or name in self.enums:
            dtype = SCALARS.get(name, 'i32')
            return dict(kind='i64' if dtype in ('i64', 'u64') else 'scalar', cType=name, dtype=dtype,
                        jsType='BigInt' if dtype in ('i64', 'u64') else 'undefined' if name == 'void' else 'Number',
                        resultNormalization='BigInt.asUintN(64, rawResult)' if dtype == 'u64' else
                        'rawResult >>> 0' if dtype == 'u32' else 'none')
        if name in self.structs:
            return dict(kind='aggregate', cType=name, pod=self.pod(name), requirement='pointer-out-adapter')
        if name.startswith('struct ') or name.startswith('mjs'):
            return dict(kind='opaque', cType=name, requirement='raw-only-lifecycle-contract')
        return dict(kind='unknown', cType=name, requirement='needs-adapter-or-policy-review')

    def describe(self, fn):
        result = self.classify(fn.return_meta or {})
        params = [dict(name=name, **self.classify(node)) for name, node in zip(fn.param_names, fn.param_meta or [])]
        kinds = {p['kind'] for p in params} | {result['kind']}
        def restrictions(node):
            found = {node['kind']} & {'callback', 'opaque', 'unknown'}
            if 'pointee' in node:
                found.update(restrictions(node['pointee']))
            return found
        for node in [result, *params]:
            kinds.update(restrictions(node))
        if len(params) != len(fn.param_names):
            kinds.add('unknown')
        if 'unknown' in kinds:
            call = 'needs-policy-review'
        elif 'callback' in kinds:
            call = 'needs-callback-bridge'
        elif 'opaque' in kinds:
            call = 'raw-only-lifecycle-contract'
        elif 'aggregate' in kinds:
            call = 'needs-aggregate-adapter'
        elif 'pointer' in kinds:
            call = 'pointer-memory-contract'
        elif 'i64' in kinds:
            call = 'BigInt-required'
        else:
            call = 'scalar'
        return dict(rawSymbol='mjwf_' + fn.name, returnType=result, parameters=params, rawCall=call,
                    exportedDoesNotImplyObjectMarshalling=True)


def generate_capabilities(functions, structs, enums, function_type):
    """Return stable machine-readable policy and additive C declarations."""
    policy = SignaturePolicy(structs, enums)
    descriptions, adapters, records = {}, [], set()
    raw_names = {fn.name for fn in functions}
    for fn in functions:
        entry = policy.describe(fn)
        result = entry['returnType']
        params = entry['parameters']
        nodes = [result, *params]
        aggregates = [n for n in nodes if n['kind'] == 'aggregate']
        safe = entry['rawCall'] == 'needs-aggregate-adapter' and aggregates and all(n.get('pod') for n in aggregates) and all(n['kind'] in ('scalar', 'i64', 'pointer', 'aggregate') for n in nodes)
        if safe and len(params) == len(fn.param_names):
            aggregate_input = any(p['kind'] == 'aggregate' for p in params)
            aggregate_output = result['kind'] == 'aggregate'
            name = fn.name + ('_ptr' if aggregate_input else '') + ('_out' if aggregate_output else '')
            declarations, names, call_names = [], [], []
            if aggregate_output:
                declarations.append(f"{fn.return_type}* mjwf_out")
                names.append('mjwf_out')
            for decl, pname, param in zip(fn.param_decls, fn.param_names, params):
                declarations.append(f"const {param['cType']}* {pname}" if param['kind'] == 'aggregate' else decl)
                names.append(pname)
                call_names.append('*' + pname if param['kind'] == 'aggregate' else pname)
            expression = f"{fn.name}({', '.join(call_names)})"
            if aggregate_output:
                expression = '*mjwf_out = ' + expression
            adapter = function_type(name, 'void' if aggregate_output else fn.return_type, declarations, names,
                                    call_expression=expression)
            adapters.append(adapter)
            records.update(n['cType'] for n in aggregates)
            entry['adapter'] = dict(symbol='mjwf_' + name, returnDecl=adapter.return_type,
                                    paramDecls=declarations, contract='caller-allocated-aligned-live-POD-memory; no JS-object marshalling')
        descriptions[fn.name] = entry
    layouts = {}
    for name in sorted(records):
        helpers = dict(sizeof='mjwf_sizeof_' + name, alignment='mjwf_alignof_' + name)
        adapters.extend([
            function_type('sizeof_' + name, 'int', [], [], call_expression=f'sizeof({name})'),
            function_type('alignof_' + name, 'int', [], [], call_expression=f'_Alignof({name})'),
        ])
        fields = {}
        for field in structs[name]['fields']:
            offset_name = 'offsetof_' + name + '_' + field['name']
            adapters.append(function_type(offset_name, 'int', [], [], call_expression=f"offsetof({name}, {field['name']})"))
            fields[field['name']] = dict(type=field['type'], offsetSymbol='mjwf_' + offset_name)
        layouts[name] = dict(**helpers, fields=fields)
    extra_names = [fn.name for fn in adapters]
    if len(set(extra_names)) != len(extra_names) or raw_names.intersection(extra_names):
        raise ValueError('Generated additive ABI name collision')
    return dict(schemaVersion=1, target='wasm32-emscripten', functions=descriptions, PODLayouts=layouts,
                generatedExtraSymbols=['mjwf_' + n for n in extra_names],
                scope='Raw C ABI; callbacks, opaque ownership and unknown signatures are not JS bridges'), adapters

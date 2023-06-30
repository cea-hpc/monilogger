from click import echo
import clang.cindex
from clang.cindex import Cursor, CursorKind
import typing
import json
from include_parser import parse_includes


# clang.cindex.TranslationUnit does not have all latest flags
# see: https://clang.llvm.org/doxygen/group__CINDEX__TRANSLATION__UNIT.html#gab1e4965c1ebe8e41d71e90203a723fe9
CXTranslationUnit_None = 0x0
CXTranslationUnit_DetailedPreprocessingRecord = 0x01
CXTranslationUnit_Incomplete = 0x02
CXTranslationUnit_PrecompiledPreamble = 0x04
CXTranslationUnit_CacheCompletionResults = 0x08
CXTranslationUnit_ForSerialization = 0x10
CXTranslationUnit_CXXChainedPCH = 0x20
CXTranslationUnit_SkipFunctionBodies = 0x40
CXTranslationUnit_IncludeBriefCommentsInCodeCompletion = 0x80
CXTranslationUnit_CreatePreambleOnFirstParse = 0x100
CXTranslationUnit_KeepGoing = 0x200
CXTranslationUnit_SingleFileParse = 0x400
CXTranslationUnit_LimitSkipFunctionBodiesToPreamble = 0x800
CXTranslationUnit_IncludeAttributedTypes = 0x1000
CXTranslationUnit_VisitImplicitAttributes = 0x2000
CXTranslationUnit_IgnoreNonErrorsFromIncludedFiles = 0x4000
CXTranslationUnit_RetainExcludedConditionalBlocks = 0x8000


default_parser_options = (
    CXTranslationUnit_DetailedPreprocessingRecord |  # needed for preprocessing parsing
    CXTranslationUnit_SkipFunctionBodies |  # for faster parsing
    # CXTranslationUnit_SingleFileParse |  # don't parse include files recursively
    CXTranslationUnit_RetainExcludedConditionalBlocks |  # keep includes inside ifdef blocks
    CXTranslationUnit_KeepGoing  # don't stop on errors
)


def filter_node_list_by_node_kind(
        nodes: typing.Iterable[Cursor],
        kinds: list,
        filepath: str
    ) -> typing.Iterable[Cursor]:
    result = []
    for i in nodes:
        if i.location.file == None:
            pass
        elif i.location.file.name == filepath:
            if i.kind in kinds:
                result.append(i)
            else:
                result += filter_node_list_by_node_kind(
                        i.get_children(),
                        kinds,
                        filepath)
    return result


def fully_qualified(c: Cursor):
    if c is None:
        return ''
    elif c.kind == CursorKind.TRANSLATION_UNIT:
        return ''
    else:
        res = fully_qualified(c.semantic_parent)
        if res != '':
            return res + '::' + c.spelling
    return c.spelling


def get_include(include_paths, filepath):
    echo(filepath)
    for p in include_paths:
        echo(p)
    path = ""
    for p in include_paths:
        if p in filepath:
            path = filepath.replace(p, "")
            break
    if path[0] == '/':
        path = path[1:]
    print(path)
    return path


def find_classes(nodes, source_file):
    return filter_node_list_by_node_kind(nodes,
                [CursorKind.CLASS_DECL, CursorKind.STRUCT_DECL], source_file)


def find_method_class(method):
    return method.semantic_parent


def find_methods(nodes, source_file):
    filter_node_list_by_node_kind(nodes,
                    [CursorKind.CXX_METHOD, CursorKind.OBJC_CLASS_METHOD_DECL, CursorKind.OBJC_INSTANCE_METHOD_DECL], source_file)


def create_struct_from_method(method):
    struct = {}
    struct['class'] = '::'.join(fully_qualified(method).split('::')[:-1])
    struct['method'] = method.spelling
    struct['name'] = f"{''.join(map(lambda s: s.capitalize(), fully_qualified(method).split('::')))}ExecutionContext"
    locals = []
    for a in method.get_arguments():
        t = a.type
        locals.append({'type': t.spelling, 'name': a.spelling})
    struct['locals'] = locals
    return struct


def parse(compile_commands_path, paths):
    fileContent = ''
    with open(compile_commands_path, 'r') as file:
        fileContent = file.read()
    data = json.loads(fileContent)
    compile_args = {}
    for d in data:
        command_args = d['command'].split(' ')[1:]
        include_args = []
        for idx, arg in enumerate(command_args):
            if arg.startswith('-I'):
                include_args.append(arg)
            elif arg.startswith('-i'):
                include_args.append(arg)
                include_args.append(command_args[idx + 1])
        compile_args[d['file']] = include_args
    
    class_to_methods = {}
    path_to_includes = {}
    gen_targets = []
    index = clang.cindex.Index.create()
    for f in paths:
        includes = []

        echo(f"Parsing {f}")
        
        translation_unit = index.parse(f, args=compile_args[f], options=default_parser_options)
        for diag in translation_unit.diagnostics:
            print(diag)
        
        includes += [str(x) for x in parse_includes(translation_unit, f)]
        includes = list(set(includes))
        includes.sort()

        all_nodes = translation_unit.cursor.get_children()
        all_methods = list([m for m in all_nodes if m.location.file != None and m.location.file.name == f and m.kind == CursorKind.CXX_METHOD])
        
        for m in all_methods:
            method_class = find_method_class(m)
            if method_class in class_to_methods:
                class_to_methods[method_class].append(m)
            else:
                class_to_methods[method_class] = [m]
        
        path_to_includes[f] = includes
    
    for c in class_to_methods:
        gen_target = {}
        structs = []
        includes = []
        
        for m in class_to_methods[c]:
            struct = create_struct_from_method(m)
            structs.append(struct)
            includes += path_to_includes[m.location.file.name]
        
        includes = list(set(includes))
        includes.sort()

        gen_target['structs'] = structs
        gen_target['qualified_name'] = fully_qualified(c).split('::')
        gen_target['includes'] = includes
        gen_targets.append(gen_target)
    
    return gen_targets

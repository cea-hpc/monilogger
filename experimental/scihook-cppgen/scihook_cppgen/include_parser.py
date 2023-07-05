from enum import Enum
from clang.cindex import CursorKind

class IncludeForm(Enum):
    Quoted = 0
    AngleBracket = 1

class IncludeInfo:
    def __init__(self, path, form, file=None):
        self.path = path
        self.form = form
        self.file = file

    def __str__(self):
        open_bracket, close_bracket = ('<', '>') if self.form == IncludeForm.AngleBracket else ('"', '"')
        return f'#include {open_bracket}{self.path}{close_bracket}'

def try_get_included_file(node):
    try:
        return node.get_included_file()
    except:
        return None

def parse_includes(translation_unit, file):
    for node in translation_unit.cursor.get_children():
        if node.location.file != None and node.location.file.name == file and node.kind == CursorKind.INCLUSION_DIRECTIVE:
            yield IncludeInfo(
                node.displayname,
                IncludeForm.AngleBracket if list(node.get_tokens())[-1].spelling == '>' else IncludeForm.Quoted,
                try_get_included_file(node)
            )
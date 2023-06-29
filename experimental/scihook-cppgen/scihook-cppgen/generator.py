from mako.template import Template
from click import echo
import os
import sys

def generate_context_file(output_path, module_name, includes, structs, dry_run):
    tpl = Template(filename=f'{os.path.dirname(os.path.abspath(sys.argv[0]))}/contexts_template.mako')
    text = tpl.render(
            header=output_path.split('/')[-1].upper().replace('.', '_'),
            module_name=module_name,
            includes=includes, structs=structs
    )
    if dry_run:
        echo(text)
    else:
        with open(output_path, 'w') as f:
            f.write(text)
from mako.template import Template
from click import echo
import os
import sys

def generate_context_file(qualified_name, includes, structs, dry_run):
    path_to_templates = f'{os.path.dirname(os.path.abspath(sys.argv[0]))}/'

    tpl_contexts = Template(filename=f'{path_to_templates}/contexts_template.mako')
    text_contexts = tpl_contexts.render(
            qualified_name=qualified_name,
            includes=includes,
            structs=structs,
            header=f"{'_'.join([s.upper() for s in qualified_name])}EXECUTIONCONTEXTS_H"
    )

    tpl_triggers = Template(filename=f'{path_to_templates}/trigger_template.mako')
    text_triggers = tpl_triggers.render(
            qualified_name=qualified_name,
            structs=structs,
            base_event=f"{'_'.join([s.upper() for s in qualified_name])}"
    )
    if dry_run:
        echo(text_contexts)
        echo(text_triggers)
    else:
        with open(f"{qualified_name[-1]}ExecutionContexts.h", 'w') as f:
            echo(f"Generating {qualified_name[-1]}ExecutionContexts.h")
            f.write(text_contexts)
        with open(f"{qualified_name[-1]}Triggers.h", 'w') as f:
            echo(f"Generating {qualified_name[-1]}Triggers.h")
            f.write(text_triggers)
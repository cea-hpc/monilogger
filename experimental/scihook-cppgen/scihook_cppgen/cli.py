#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""CLI for `scihook` package."""
import parser
import generator

import click
from click import echo, confirm
# from importlib_resources import files

# from scihook import __version__

from re import sub
def snake_case(s):
  return '_'.join(sub('([A-Z][a-z]+)', r' \1', sub('([A-Z]+)', r' \1', s.replace('-', ' '))).split()).lower()


@click.group()
@click.help_option(help="Show this message and exit")
# @click.version_option(__version__, help="Show the version and exit")
@click.pass_context
def scihook(ctx):
    """
    An instrumentation code generation tool for SciHook.
    """
    pass


@scihook.command()
@click.option("--dry-run", "-dr", is_flag=True, help="Print file content to be generated instead of generating it")
@click.option("--output-path", "-o", help="Output path of generated files (defaults to current folder)", default='.')
@click.option("--include-prefix", "-ip", help="Include path prefix for generated files (defaults to empty path)", default='')
@click.argument("compile-commands-path")
@click.argument("source-files-paths", nargs=-1)
@click.pass_context
def genctx(ctx, dry_run, output_path, include_prefix, compile_commands_path, source_files_paths):
    gen_targets = parser.parse(compile_commands_path, source_files_paths)
    for t in gen_targets:
        generator.generate_context_file(
            t['qualified_name'],
            t['includes'],
            t['structs'],
            output_path,
            include_prefix,
            dry_run)

scihook.add_command(genctx)

if __name__ == '__main__':
    scihook()

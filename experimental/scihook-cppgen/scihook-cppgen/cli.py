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
@click.option("--compile-commands", "-cc", help="Path to compile_commands.json")
@click.option("--dry-run", "-dr", is_flag=True, help="Print file content to be generated instead of generating it")
@click.argument("filename")
@click.argument("paths", nargs=-1)
@click.pass_context
def genctx(ctx, compile_commands, dry_run, filename, paths):
    includes, structs = parser.parse(compile_commands, paths)
    generator.generate_context_file(filename, snake_case(filename.split('/')[-1].split('.')[0]), includes, structs, dry_run)

scihook.add_command(genctx)

if __name__ == '__main__':
    scihook()

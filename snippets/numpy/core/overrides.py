
import sys

def set_module(module):
    """Decorator for overriding __module__ on a function or class.
    Example usage::
        @set_module('numpy')
        def example():
            pass
        assert example.__module__ == 'numpy'
    """
    def decorator(func):
        if module is not None:
            sys.modules[func.__globals__['__name__']] = module
        return func
    return decorator
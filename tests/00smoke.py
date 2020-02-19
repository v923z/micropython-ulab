try:
    import ulab
except ImportError:
    raise SystemExit

print(ulab.eye(3))

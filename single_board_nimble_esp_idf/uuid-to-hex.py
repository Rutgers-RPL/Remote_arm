import uuid
import sys

if len(sys.argv) != 2:
    print(f"Usage: {sys.argv[0]} <uuid>")
    sys.exit(1)

u_str = sys.argv[1]

try:
    u = uuid.UUID(u_str)
except ValueError:
    print(f"Invalid UUID: {u_str}")
    sys.exit(1)

be = u.bytes
le = u.bytes_le

print(f"\nUUID: {u}\n")

print("Big-endian:")
print(f"{', '.join(f'0x{b:02x}' for b in be)}\n")

print(f"Little-endian (NimBLE):")
print(f"{', '.join(f'0x{b:02x}' for b in le)}\n")

print(f"Macro (NimBLE):\nBLE_UUID128_INIT({', '.join(f'0x{b:02x}' for b in le)});\n")
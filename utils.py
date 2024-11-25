def read_file_bits(fpath: str = "zipped.bin") -> None:
    with open(fpath, 'rb') as f:
        msg = []
        while 1:
            byte_s = f.read(1)
            if not byte_s:
                break
            byte = byte_s[0]
            msg.append(f"{byte:08b}")

    print("".join(msg))

def read_huffman_code(fpath: str = "tmp/code.txt") -> None:
    with open(fpath, 'r') as f:
        codes = f.read().splitlines() 
        for i, code in enumerate(codes):
            if not code:
                continue
            print(f"'{chr(i)}' ({i}): {code}")

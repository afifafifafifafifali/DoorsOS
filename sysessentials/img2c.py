from PIL import Image
import sys

def image_to_c_array(input_path, output_path, array_name="image_data", grayscale=False):

    img = Image.open(input_path)

    img = img.resize((64, 64), Image.NEAREST)
    if grayscale:
        img = img.convert("L")  
    else:
        img = img.convert("RGB")  

    pixels = list(img.getdata())

    with open(output_path, "w") as f:
        if grayscale:
            f.write(f"unsigned char {array_name}[64][64] = {{\n")
            for y in range(64):
                f.write("    {")
                for x in range(64):
                    val = pixels[y * 64 + x]
                    f.write(f"{val}")
                    if x != 63:
                        f.write(", ")
                f.write("}")
                if y != 63:
                    f.write(",")
                f.write("\n")
            f.write("};\n")
        else:
            f.write(f"unsigned char {array_name}[64][64][3] = {{\n")
            for y in range(64):
                f.write("    {")
                for x in range(64):
                    r, g, b = pixels[y * 64 + x]
                    f.write(f"{{{r}, {g}, {b}}}")
                    if x != 63:
                        f.write(", ")
                f.write("}")
                if y != 63:
                    f.write(",")
                f.write("\n")
            f.write("};\n")

    print(f"Saved C array to {output_path}")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python script.py input_image output.c [gray]")
        sys.exit(1)

    input_image = sys.argv[1]
    output_file = sys.argv[2]
    grayscale = len(sys.argv) > 3 and sys.argv[3].lower() == "gray"

    image_to_c_array(input_image, output_file, grayscale=grayscale)
    #Usage python img2c.py input.png output.c 
    #usage python img2c.py input.png output.c gray
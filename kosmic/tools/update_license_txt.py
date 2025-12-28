with open("../src/license_txt_gen.h", "w") as f_out:
    f_out.write("// Auto-generated file using \"tools/update_license_txt.py\" and LICENSE.txt:\n")
    f_out.write("const char license_txt[] = \n")
    with open("../LICENSE.txt") as f_in:
        for line in f_in:
            line = "\"" + line.rstrip().replace("\"", "\\\"") + "\\n\""
            f_out.write(line + "\n")
    f_out.write(";\n")

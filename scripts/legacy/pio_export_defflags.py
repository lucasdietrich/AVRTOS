import re
import os, os.path

def get_envs(filename: str):
    rec_env = re.compile(r"\[env:(?P<env>[a-zA-Z0-9-_]*)\]")
    rec_bflags = re.compile(r"build_flags(\s*)=(\s*)")
    rec_def = re.compile(r"[\t\s]*-D(?P<name>CONFIG_[A-Z0-9_]*)(\s)*=(\s)*(?P<value>(-?)(0x)?[0-9]*)")
    rec_sample = re.compile(r"[\t\s]*\+<examples/(?P<sample>[a-zA-Z-_]*)(/?)>")

    NONE, ENV, DEFINES = 0, 1, 2
    envs = []
    state = NONE
    env = dict()
    
    with open(filename, "r+") as fp:
         pioini = fp.readlines()

    for line in pioini:
        menv = rec_env.match(line)
        if menv:
            state = ENV
            env = {
                "name": menv.group("env"),
                "sample_dir": "",
                "defines": list()
            }
            envs.append(env)
        elif state is ENV:
            if rec_bflags.match(line):
                state = DEFINES
        elif state is DEFINES:

            mdef = rec_def.match(line)
            if mdef:
                env["defines"].append(
                    (mdef.group("name"), mdef.group("value"))
                )

        samplem = rec_sample.match(line)
        if env is not None and rec_sample.match(line):
            env["sample_dir"] = samplem.group("sample")
    
    return envs
    
def export_envs(envs):
    export = ""
    for env in envs:
        export += f"\n[env: {env['name']} src/examples/{env['sample_dir']}]\n"

        for define in env["defines"]:
            export += f"{define[0]}={define[1]}\n"

    return export

def generate_cmakelists(envs):
    for env in envs:
        sample_name = env["sample_dir"]
        if sample_name != "":
            path = os.path.abspath(f"./src/examples/{sample_name}")
        
        begin = """
add_executable(${PROJECT_NAME} main.c)

# AVRTOS Configuration
target_compile_definitions(${PROJECT_NAME} PUBLIC
"""

        end = """
)

target_link_avrtos(${PROJECT_NAME})

target_prepare_env(${PROJECT_NAME})
"""

        cmakelists_path = os.path.join(path, "CMakeLists.txt")

        with open(cmakelists_path, "w+") as fp:
            fp.write(f"project(sample_{sample_name})")
            fp.write(begin)

            for define in env["defines"]:
                fp.write(f"\t{define[0]}={define[1]}\n")

            fp.write(end)

if __name__ == "__main__":
    envs = get_envs("./platformio.ini")

    export = export_envs(envs)

    with open("./defs_export.txt", "w+") as fp:
        fp.write(export)

    generate_cmakelists(envs)
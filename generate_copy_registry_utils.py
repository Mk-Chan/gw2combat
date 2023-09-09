import os
import subprocess

import clang.cindex

clang.cindex.Config.set_library_path("/opt/homebrew/opt/llvm/lib")


def find_classes_in_namespace(cursor, namespace, components, tags):
    found_any = False
    if cursor.kind == clang.cindex.CursorKind.NAMESPACE:
        namespace = f"{namespace}::{cursor.spelling}" if namespace else cursor.spelling
    elif cursor.kind in (clang.cindex.CursorKind.STRUCT_DECL, clang.cindex.CursorKind.CLASS_DECL) \
            and namespace == "gw2combat::component":
        has_non_static_members = any(
            c.kind == clang.cindex.CursorKind.FIELD_DECL for c in cursor.get_children())
        if has_non_static_members:
            components.append(f"{namespace}::{cursor.spelling}")
        else:
            tags.append(f"{namespace}::{cursor.spelling}")
        found_any = True
        namespace += "::" + cursor.spelling

    for c in cursor.get_children():
        found_any |= find_classes_in_namespace(c, namespace, components, tags)
    return found_any


def extract_classes_from_cpp_files(directory):
    idx = clang.cindex.Index.create()

    includes = []
    components = []
    tags = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".cpp") or file.endswith(".h") or file.endswith(".hpp"):
                filepath = os.path.join(root, file)
                tu = idx.parse(filepath)
                found_any = find_classes_in_namespace(tu.cursor, "", components, tags)
                if found_any:
                    includes.append(f"#include \"{filepath.replace('src/', '')}\"")

    return includes, components, tags


def format_file(filename):
    try:
        result = subprocess.run(['clang-format', '-i', filename], check=True)
        if result.returncode == 0:
            print(f"Formatted {filename} successfully")
        else:
            print(f"Failed to format {filename}")
    except Exception as e:
        print(f"An error occurred while formatting {filename}: {e}")


def main():
    directory = "src/component"
    includes, components, tags = extract_classes_from_cpp_files(directory)

    code = """#include "registry_utils.hpp"

    #include "entity_utils.hpp"

    """
    code += "\n".join(includes) + "\n"
    code += """
    namespace gw2combat::utils {

    void copy_registry(registry_t& source_registry, registry_t& destination_registry) {
        destination_registry.ctx().emplace<tick_t>(source_registry.ctx().get<tick_t>());
    """
    code += """
        source_registry.each([&](auto entity) {
            // Create entity and copy name
            auto destination_entity = destination_registry.create(entity);
            destination_registry.ctx().emplace_as<std::string>(destination_entity, utils::get_entity_name(entity, source_registry));
    """

    code += """

    // Copy components"""
    for component in components:
        code += f"""
            if(source_registry.all_of<{component}>(entity)) {{
                destination_registry.emplace<{component}>(destination_entity, source_registry.get<{component}>(entity));
            }}"""

    code += """

    // Copy tags"""
    for tag in tags:
        code += f"""
            if(source_registry.all_of<{tag}>(entity)) {{
                destination_registry.emplace<{tag}>(destination_entity);
            }}"""

    code += """
        });
    }

    } // namespace gw2combat::utils
    """

    filename = "src/utils/registry_utils.cpp"
    with open(filename, "w") as f:
        f.write(code)
    print("Wrote", filename, "successfully")
    format_file(filename)


if __name__ == "__main__":
    main()

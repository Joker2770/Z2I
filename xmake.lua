add_requires("onnxruntime 1.11.1")
add_includedirs("src", {public = true})

set_languages("c11", "cxx17")

target("Z2I")
    set_kind("static")
    add_files("src/*.cpp")
    add_packages("onnxruntime")

target("pbrain-Z2I")
    set_kind("binary")
    add_files("src/pbrain-Z2I/*.cpp")
    add_deps("Z2I")
    add_packages("onnxruntime")

target("train_and_eval")
    set_kind("binary")
    add_files("src/train_and_eval/*.cpp")
    add_deps("Z2I")
    add_packages("onnxruntime")

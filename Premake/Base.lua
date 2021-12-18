function make_path(str)
    local result = str
    if (os.target() == "windows") then
        result = str:gsub("/", "\\")
    end

    return result
end
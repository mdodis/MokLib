function make_path(str)
    if (str == nil) then
        return nil
    end

    local result = str
    if (os.target() == "windows") then
        result = str:gsub("/", "\\")
    end

    return result
end
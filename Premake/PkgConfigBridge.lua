function splitstr(s, delim)
    local result = {};
    for match in (s..delim):gmatch("(.-)"..delim) do
        table.insert(result, match)
    end
    return result;
end


local function parse_mod_libs(mod)
    local linkdirs = {}
    local links = {}

    local libs = splitstr(mod.libs, ' ')
    for _,s in pairs(libs) do
        if (s:find("^-L")) ~= nil then
            local str = s:gsub("-L", "");
            table.insert(linkdirs, str)
        elseif (s:find("^-l")) ~= nil then
            local str = s:gsub("-l", "");
            table.insert(links, str)
        end
    end

    mod.links = links
    mod.linkdirs = linkdirs
end

function pkgload(module)
    local pkgconfig = require "pkgconfig"
    local mod = pkgconfig.parse(pkgconfig.load(module))
    parse_mod_libs(mod)
    return mod
end

function pkgadd(name)
    local mod = pkgload(name)
    includedirs(mod.cflags)
    libdirs(mod.linkdirs)
    links(mod.links)
end
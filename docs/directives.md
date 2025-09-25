## Directives

- listen INT {server}
- host STRING {server}
- server_name STRING {server}
- root STRING {server, location}
- index STRING[] {server, location}
- error_page INT STIRNG {server, location}
- client_max_body_size SIZE {server, location}
- autoindex BOOL {location}
- allowed_methods STRING[] {location}
- cgi_pass STRING {location}
- cgi_ext STRING[] {location}
- cgi_timout INT {location}
- upload_enabled BOOL {location}
- upload_store STRING {location}
- redirect INT STRING {location}

struct Directives
{
    
}


LocationConfig lcocation;
location["index"]
Decl operator[](std::string const & key) -> ConfigValue &;
{
    declaration_map_t::iterator it = declarations.find(key);
    if (it == declarations.end())
        serverconfig[key] = Declaration(key);
}

IntDecl get()
StringDecl
BoolDecl
SizeDecl
StringArrayDecl

auto decl = location[root].get();

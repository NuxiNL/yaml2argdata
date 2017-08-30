# YAML to Argdata conversion classes

[Argdata](https://github.com/NuxiNL/argdata) is a binary serialisation
format that is structurally similar to YAML. The main difference is that
Argdata allows for the embedding of POSIX file descriptors. This makes
it useful for [CloudABI](https://nuxi.nl/cloudabi/) to pass
configuration and resources to programs on startup.
[ARPC](https://github.com/NuxiNL/arpc) also uses Argdata to make it
possible to send RPCs to processes with file descriptors attached.

There are cases in which we want to convert YAML to Argdata, as done by
[the `cloudabi-run` utility](https://github.com/NuxiNL/cloudabi-utils)
and [Scuba](https://github.com/NuxiNL/scuba). To ensure that these tools
process YAML in a uniform way, this package provides some wrappers
around [yaml-cpp](https://github.com/jbeder/yaml-cpp) to do this
conversion.

These classes allow for the conversion of all YAML nodes, with the
exception of ones that inject file descriptors. This is because file
descriptor injection is specific to the context in which the YAML is
parsed. For example, `cloudabi-run` allows you to open arbitrary paths
on disk, whereas Scuba doesn't support this. Conversely, Scuba allows
you to interact with Kubernetes' networking. This functionality will
need to be added in the form of a separate adapter class.

# Example usage

```cpp
#include <yaml2argdata/yaml_argdata_factory.h>
#include <yaml2argdata/yaml_builder.h>
#include <yaml2argdata/yaml_canonicalizing_factory.h>
#include <yaml2argdata/yaml_error_factory.h>
#include <yaml2argdata/yaml_factory.h>

using yaml2argdata::YAMLArgdataFactory;
using yaml2argdata::YAMLBuilder;
using yaml2argdata::YAMLCanonicalizingFactory;
using yaml2argdata::YAMLErrorFactory;
using yaml2argdata::YAMLFactory;

class MyOwnFileDescriptorFactory : public YAMLFactory<const argdata_t*> {
  ...
};

... {
  YAMLErrorFactory<const argdata_t*> error_factory;
  MyOwnFileDescriptorFactory file_descriptor_factory(&error_factory);
  YAMLArgdataFactory argdata_factory(&file_descriptor_factory);
  YAMLCanonicalizingFactory<const argdata_t*> canonicalizing_factory(
      &argdata_factory);
  YAMLBuilder<const argdata_t*> builder(&canonicalizing_factory);
  std::istream* argdata_stream = ...;
  const argdata_t* argdata = builder.Build(argdata_stream);
}
```

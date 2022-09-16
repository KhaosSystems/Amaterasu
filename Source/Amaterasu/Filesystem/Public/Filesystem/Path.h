#include "Kernel/String.h"

namespace Filesystem
{
    class Path
    {
    public:
        Path& Normalize();

    private:
        Kernel::WString path;
    };
}
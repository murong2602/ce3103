#include <linux/init.h>     // Needed for module_init and module_exit macros
#include <linux/module.h>   // Needed for all kernel modules
#include <linux/kernel.h>   // Needed for KERN_ macros

// Initialization function - called when the module is loaded
static int __init hello_init(void)
{
    printk(KERN_ALERT "Hello from kernel world\n");
    return 0;  // Return 0 for success
}

// Cleanup function - called when the module is removed
static void __exit hello_exit(void)
{
    printk(KERN_ALERT "Goodbye from kernel world\n");
}

// Macros to define the initialization and cleanup functions
module_init(hello_init);
module_exit(hello_exit);

// Module metadata
MODULE_LICENSE("GPL");          // License for the module
MODULE_AUTHOR("CE3103");        // Author information
MODULE_DESCRIPTION("Simple Hello module");
MODULE_VERSION("V1");

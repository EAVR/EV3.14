#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x708ffad, "module_layout" },
	{ 0x41344088, "param_get_charp" },
	{ 0x6ad065f4, "param_set_charp" },
	{ 0x72becf51, "hrtimer_start" },
	{ 0xc9cbfbd7, "hrtimer_init" },
	{ 0x93fca811, "__get_free_pages" },
	{ 0xa3413eb6, "misc_register" },
	{ 0xde75b689, "set_irq_type" },
	{ 0xfda85a7d, "request_threaded_irq" },
	{ 0xbbd45ed4, "hrtimer_forward" },
	{ 0x788fe103, "iomem_resource" },
	{ 0x9ad0d04a, "davinci_ioremap" },
	{ 0xadf42bd5, "__request_region" },
	{ 0xc741a87f, "da8xx_syscfg0_base" },
	{ 0xea147363, "printk" },
	{ 0x765f75fb, "pgprot_user" },
	{ 0x3145351, "remap_pfn_range" },
	{ 0x2df234a3, "mem_map" },
	{ 0x37a0cba, "kfree" },
	{ 0xa24d2473, "davinci_iounmap" },
	{ 0xaece669e, "misc_deregister" },
	{ 0x4a5361cf, "hrtimer_cancel" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x11f447ce, "__gpio_to_irq" },
	{ 0x701d0ebd, "snprintf" },
	{ 0x17a142df, "__copy_from_user" },
	{ 0xfa2a45e, "__memzero" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0x2196324, "__aeabi_idiv" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x2e712c61, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x2b131b84, "__register_chrdev" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x5f754e5a, "memset" },
	{ 0xc5850110, "printk" },
	{ 0xe97c4103, "ioremap" },
	{ 0x12a38747, "usleep_range" },
	{ 0x2cfde9a2, "warn_slowpath_fmt" },
	{ 0xedc03953, "iounmap" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "7B4BF95BADE481C0726A94E");

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_DESCRIPTION("Kernel module for understanding dependency orderings in unoptimised IR");
MODULE_AUTHOR("Paul Heidekruger");
// MODULE_LICENSE("GPL");

// Most basic data dependencies

static int data_read_write(void)
{
	// Declaration
	int x, y, z;

	// Definition
	WRITE_ONCE(x, 42);

	// Begin data dependency
	y = READ_ONCE(x);

	// Do stuff

	// End data dependency
	WRITE_ONCE(z, y);

	return 0;
}

static int data_read_write_addition(void)
{
	// Declaration
	int x, y, z;

	// Definition
	WRITE_ONCE(x, 42);

	// Begin data Dependency
	y = READ_ONCE(x);

	// Do stuff

	// End data dependency
	WRITE_ONCE(z, y + 3);

	return 0;
}

// How does call by value look in IR?
static int data_read_write_across_boundaries(int y)
{
	// Declaration
	int z;

	// End data dependency
	WRITE_ONCE(z, y);

	return 0;
}

// source: https://linuxplumbersconf.org/event/7/contributions/821/attachments/598/1075/LPC_2020_--_Dependency_ordering.pdf - slide 4
static int address_to_control(void) 
{
	// Declaration
	static int foo[50];
	const volatile int* bar;
	const volatile int* x;
	int y;

	// Begin address dependency
	// x == arr && *x == arr[0] after assignment
	x = READ_ONCE(foo);

	// bar == x + 42 && *bar == x[42] after assignment - random value, since arr is not initialised
	bar = &x[42];

	// End data dependency
	// y == x[42]
	y = READ_ONCE(*bar);

	return 0;
}

// static int data_read_store_release(void)
// {
// 	// Declaration
// 	int x, y, z;

// 	// Definition
// 	WRITE_ONCE(x, 42);

// 	// Begin data dependencies
// 	y = READ_ONCE(x);

// 	// Do stuff

// 	// End data dependency
// 	smp_store_release(&z, y);

// 	return 0;
// }

// static int data_read_store_mb(void)
// {
// 	// Declaration
// 	int x, y, z;

// 	// Definition
// 	WRITE_ONCE(x, 42);

// 	// Begin data dependencies
// 	y = READ_ONCE(x);

// 	// Do stuff

// 	// End data dependency
// 	smp_store_mb(z, y);

// 	return 0;
// }

// static int data_load_acquire_write(void)
// {
// 	// Declaration
// 	int x, y, z;

// 	// Definition
// 	WRITE_ONCE(x, 42);

// 	// Begin data dependencies
// 	y = smp_load_acquire(&x);

// 	// Do stuff

// 	// End data dependency
// 	WRITE_ONCE(z, y);

// 	return 0;
// }

// static int data_load_acquire_store_release(void)
// {
// 	// Declaration
// 	int x, y, z;

// 	// Definition
// 	WRITE_ONCE(x, 42);

// 	// Begin data dependencies
// 	y = smp_load_acquire(&x);

// 	// Do stuff

// 	// End data dependency
// 	smp_store_release(&z, y);

// 	return 0;
// }

// static int data_load_acquire_store_mb(void)
// {
// 	// Declaration
// 	int x, y, z;

// 	// Definition
// 	WRITE_ONCE(x, 42);

// 	// Begin data dependencies
// 	y = smp_load_acquire(&x);

// 	// Do stuff

// 	// End data dependency
// 	smp_store_mb(z, y);

// 	return 0;
// }

// static int data_rcu_dereference_write(void)
// {
//	// Declaration
//	int x, y, z;

//	// Definition
//	WRITE_ONCE(x, 42);

//	// Begin data dependencies
//	y = rcu_dereference(x);

//	// Do stuff

//	// End data dependency
//	WRITE_ONCE(z, y);

//	return 0;
// }

// static int data_rcu_dereference_store_release(void)
// {
// 	// Declaration
// 	int x, y, z;

// 	// Definition
// 	WRITE_ONCE(x, 42);

// 	// Begin data dependencies
// 	y = rcu_dereference(x);

// 	// Do stuff

// 	// End data dependency
// 	smp_store_release(z, y);

// 	return 0;
// }

// static int data_rcu_dereference_store_mb(void)
// {
// 	// Declaration
// 	int x, y, z;

// 	// Definition
// 	WRITE_ONCE(x, 42);

// 	// Begin data dependencies
// 	y = rcu_dereference(x);

// 	// Do stuff

// 	// End data dependency
// 	smp_store_mb(z, y);

// 	return 0;
// }

// Basic data dependencies with some extras

static int lkm_init(void)
{
	int x, y;

	WRITE_ONCE(x, 42);
	y = READ_ONCE(x);

	pr_debug("Hi\n");

	data_read_write();
	data_read_write_addition();
	data_read_write_across_boundaries(y);
	address_to_control();
	// data_read_store_release();
	// data_read_store_mb();
	// data_load_acquire_write();
	// data_load_acquire_store_release();
	// data_load_acquire_store_mb();
	// data_rcu_dereference_write();
	// data_rcu_dereference_store_release();
	// data_rcu_dereference_store_mb();

	return 0;
}

static void lkm_exit(void)
{
	pr_debug("Bye\n");
}

module_init(lkm_init);
module_exit(lkm_exit);

#include <stdbool.h>
#include <stdint.h>

enum PAGE_PDE_FLAGS {
 
	I86_PDE_PRESENT			=	1,		    //0000000000000000000000000000001
	I86_PDE_WRITABLE		=	2,		    //0000000000000000000000000000010
	I86_PDE_USER			=	4,	    	//0000000000000000000000000000100
	I86_PDE_PWT			    =	8,		    //0000000000000000000000000001000
	I86_PDE_PCD			    =	0x10,		//0000000000000000000000000010000
	I86_PDE_ACCESSED		=	0x20,		//0000000000000000000000000100000
	I86_PDE_DIRTY			=	0x40,		//0000000000000000000000001000000
	I86_PDE_PAGE_SIZE	    =	0x80,		//0000000000000000000000010000000
	I86_PDE_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
   	I86_PDE_PAT				=	0x2000,
	I86_PDE_FRAME			=	0x7FFFF000 	//1111111111111111111000000000000
};
 
//! a page directery entry
typedef uint32_t pd_entry;

extern void		pd_entry_add_attrib (pd_entry* e, uint32_t attrib);
extern void		pd_entry_del_attrib (pd_entry* e, uint32_t attrib);
extern void		pd_entry_set_frame (pd_entry* e, uint32_t addr);
extern bool		pd_entry_is_present (pd_entry e);
extern bool		pd_entry_is_user (pd_entry e);
extern bool		pd_entry_is_4mb (pd_entry e);
extern bool		pd_entry_is_writable (pd_entry e);
extern uint32_t	pd_entry_pfn (pd_entry e);
extern void		pd_entry_enable_global (pd_entry *e);
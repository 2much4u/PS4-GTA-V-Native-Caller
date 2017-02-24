#define	SLIST_ENTRY(type)						\
struct {								\
	struct type *sle_next;	/* next element */			\
}

#define	SLIST_HEAD(name, type)						\
struct name {								\
	struct type *slh_first;	/* first element */			\
}

#define	TRACEBUF

#define	TAILQ_ENTRY(type)						\
struct {								\
	struct type *tqe_next;	/* next element */			\
	struct type **tqe_prev;	/* address of previous next element */	\
	TRACEBUF							\
}

struct knote;

struct kevent {
	uintptr_t	ident;		/* identifier for this event */
	short		filter;		/* filter for event */
	unsigned short		flags;
	unsigned int		fflags;
	intptr_t	data;
	void		*udata;		/* opaque user data identifier */
};

struct filterops {
	int	f_isfd;		/* true if ident == filedescriptor */
	int	(*f_attach)(struct knote *kn);
	void	(*f_detach)(struct knote *kn);
	int	(*f_event)(struct knote *kn, long hint);
	void	(*f_touch)(struct knote *kn, struct kevent *kev, unsigned long type);
};

struct knote {
	SLIST_ENTRY(knote)	kn_link;	/* for kq */
	SLIST_ENTRY(knote)	kn_selnext;	/* for struct selinfo */
	struct			knlist *kn_knlist;	/* f_attach populated */
	TAILQ_ENTRY(knote)	kn_tqe;
	struct			kqueue *kn_kq;	/* which queue we are on */
	struct 			kevent kn_kevent;
	int			kn_status;	/* protected by kq lock */
	int			kn_sfflags;	/* saved filter flags */
	intptr_t		kn_sdata;	/* saved data field */
	union {
		struct		file *p_fp;	/* file data pointer */
		struct		proc *p_proc;	/* proc pointer */
		struct		aiocblist *p_aio;	/* AIO job pointer */
		struct		aioliojob *p_lio;	/* LIO job pointer */ 
	} kn_ptr;
	struct			filterops *kn_fop;
	void			*kn_hook;
	int			kn_hookid;
};

SLIST_HEAD(klist, knote);

struct knlist {
	struct	klist	kl_list;
	void    (*kl_lock)(void *);	/* lock function */
	void    (*kl_unlock)(void *);
	void	(*kl_assert_locked)(void *);
	void	(*kl_assert_unlocked)(void *);
	void *kl_lockarg;		/* argument passed to kl_lockf() */
};

struct sendto_args {
	int	s;
	void *	buf;
	size_t	len;
	int	flags;
	void *	to;
	int	tolen;
};

struct auditinfo_addr {
	/*
	4	ai_auid;
	8	ai_mask;
	24	ai_termid;
	4	ai_asid;
	8	ai_flags;r
	*/
	char useless[184];
};

//struct ucred {
//	u_int	cr_ref;			/* reference count */
//	uid_t	cr_uid;			/* effective user id */
//	uid_t	cr_ruid;		/* real user id */
//	uid_t	cr_svuid;		/* saved user id */
//	int	cr_ngroups;		/* number of groups */
//	gid_t	cr_rgid;		/* real group id */
//	gid_t	cr_svgid;		/* saved group id */
//	struct uidinfo	*cr_uidinfo;	/* per euid resource consumption */
//	struct uidinfo	*cr_ruidinfo;	/* per ruid resource consumption */
//	struct prison	*cr_prison;	/* jail(2) */
//	struct loginclass	*cr_loginclass; /* login class */
//	u_int		cr_flags;	/* credential flags */
//	void 		*cr_pspare2[2];	/* general use 2 */
//	struct label	*cr_label;	/* MAC label */
//	struct auditinfo_addr	cr_audit;	/* Audit properties. */
//	gid_t	*cr_groups;		/* groups */
//	int	cr_agroups;		/* Available groups */
//};

struct ucred {
	uint32_t useless1;
	uint32_t cr_uid;     // effective user id
	uint32_t cr_ruid;    // real user id
	uint32_t useless2;
	uint32_t useless3;
	uint32_t cr_rgid;    // real group id
	uint32_t useless4;
	void *useless5;
	void *useless6;
	void *cr_prison;     // jail(2)
	void *useless7;
	uint32_t useless8;
	void *useless9[2];
	void *useless10;
	struct auditinfo_addr useless11;
	uint32_t *cr_groups; // groups
	uint32_t useless12;
};

struct callout {
	union {
		SLIST_ENTRY(callout) sle;
		TAILQ_ENTRY(callout) tqe;
	} c_links;
	int c_time; /* ticks to the event */
	void *c_arg; /* function argument */
	void (*c_func)(void *); /* function to call */
	void *c_lock; /* lock to handle */
	int c_flags; /* state of this entry */
	volatile int c_cpu; /* CPU we're scheduled on */
};

struct proc {
	char useless[64];
	struct ucred *p_ucred;
	void *p_fd;
	void *p_fdtol;
	void *p_stats;
	void *p_limit;
	struct callout p_limco;
	void *p_sigfacts;
	int p_flag;
	enum {
		PRS_NEW = 0, /* In creation */
		PRS_NORMAL, /* threads can be run. */
		PRS_ZOMBIE
	} p_state; /* (j/c) Process status. */
	int p_pid;
};


struct thread {
	void *useless;
	struct proc *td_proc;
};

enum uio_seg {
    UIO_USERSPACE,      /* from user data space */
    UIO_SYSSPACE,       /* from system space */
    UIO_NOCOPY      /* don't copy, already in object */
};
 
enum uio_rw {
    UIO_READ,
    UIO_WRITE
};
 
struct iovec {
    void *iov_base;  /* Base address. */
    size_t iov_len;    /* Length. */
};
 
struct uio {
    struct iovec *uio_iov;         /* scatter/gather list */
    int uio_iovcnt;             /* length of scatter/gather list */
	off_t uio_offset;             /* offset in target object */
	ssize_t uio_resid;              /* remaining bytes to copy */
	enum uio_seg uio_segflg;     /* address space */
	enum uio_rw uio_rw;          /* operation */
	struct thread *uio_td;         /* owner */
};
 

struct fileops {
	void *fo_read;
	void *fo_write;
	void *fo_truncate;
	void *fo_ioctl;
	void *fo_poll;
	void *fo_kqfilter;
	void *fo_stat;
	void *fo_close;
	void *fo_chmod;
	void *fo_chown;
	int	fo_flags;	/* DFLAG_* below */
};

#define X86_CR0_WP (1 << 16)

static inline __attribute__((always_inline)) uint64_t readCr0(void) {
	uint64_t cr0;
	
	asm volatile (
		"movq %0, %%cr0"
		: "=r" (cr0)
		: : "memory"
 	);
	
	return cr0;
}

static inline __attribute__((always_inline)) void writeCr0(uint64_t cr0) {
	asm volatile (
		"movq %%cr0, %0"
		: : "r" (cr0)
		: "memory"
	);
}

#define CTL_KERN 1
#define KERN_PROC 14
#define KERN_PROC_PID 1
#define KERN_PROC_VMMAP 10
#define FALSE 0
#define TRUE 1
#define BOOL uint8_t

#define	PT_TRACE_ME	0	/* child declares it's being traced */
#define	PT_READ_I	1	/* read word in child's I space */
#define	PT_READ_D	2	/* read word in child's D space */
/* was	PT_READ_U	3	 * read word in child's user structure */
#define	PT_WRITE_I	4	/* write word in child's I space */
#define	PT_WRITE_D	5	/* write word in child's D space */
/* was	PT_WRITE_U	6	 * write word in child's user structure */
#define	PT_CONTINUE	7	/* continue the child */
#define	PT_KILL		8	/* kill the child process */
#define	PT_STEP		9	/* single step the child */
#define	PT_ATTACH	10	/* trace some running process */
#define	PT_DETACH	11	/* stop tracing a process */
#define PT_IO		12	/* do I/O to/from stopped process. */
#define	PT_LWPINFO	13	/* Info about the LWP that stopped. */
#define PT_GETNUMLWPS	14	/* get total number of threads */
#define PT_GETLWPLIST	15	/* get thread list */
#define PT_CLEARSTEP	16	/* turn off single step */
#define PT_SETSTEP	17	/* turn on single step */
#define PT_SUSPEND	18	/* suspend a thread */
#define PT_RESUME	19	/* resume a thread */
#define	PT_TO_SCE	20
#define	PT_TO_SCX	21
#define	PT_SYSCALL	22
#define	PT_FOLLOW_FORK	23
#define PT_GETREGS      33	/* get general-purpose registers */
#define PT_SETREGS      34	/* set general-purpose registers */
#define PT_GETFPREGS    35	/* get floating-point registers */
#define PT_SETFPREGS    36	/* set floating-point registers */
#define PT_GETDBREGS    37	/* get debugging registers */
#define PT_SETDBREGS    38	/* set debugging registers */
#define	PT_VM_TIMESTAMP	40	/* Get VM version (timestamp) */
#define	PT_VM_ENTRY	41	/* Get VM map (entry) */

struct ptrace_io_desc {
	int	piod_op;	/* I/O operation */
	void *piod_offs;	/* child offset */
	void *piod_addr;	/* parent offset */
	size_t piod_len;	/* request length */
};

/*
 * Operations in piod_op.
 */
#define PIOD_READ_D	1	/* Read from D space */
#define PIOD_WRITE_D	2	/* Write to D space */
#define PIOD_READ_I	3	/* Read from I space */
#define PIOD_WRITE_I	4	/* Write to I space */

struct ptrace_vm_entry {
	int pve_entry;	/* Entry number used for iteration. */
	int pve_timestamp;	/* Generation number of VM map. */
	long pve_start;	/* Start VA of range. */
	long pve_end;	/* End VA of range (incl). */
	long pve_offset;	/* Offset in backing object. */
	int pve_prot;	/* Protection of memory range. */
	int  pve_pathlen;	/* Size of path. */
	long pve_fileid;	/* File ID. */
	uint32_t pve_fsid;	/* File system ID. */
	char *pve_path;	/* Path name of object. */
};
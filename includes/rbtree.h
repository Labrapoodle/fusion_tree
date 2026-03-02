#ifndef RBTREE_H
#define RBTREE_H
#include <stdint.h>

struct rbnode {
    struct rbnode* parent;
    struct rbnode* left;
    struct rbnode* right;
    char color; // RED=0, BLACK=1
    char value;
    uint64_t key;
};

typedef struct{
    struct rbnode* root;
    struct rbnode* nil;
}RBtree;

RBtree *rbtree_create();
void rb_add_fixup(RBtree *T,struct rbnode *z);
void left_rotate(RBtree *T, struct rbnode *x);
void right_rotate(RBtree *T, struct rbnode *x);
void rbtree_transplant(RBtree *T, struct rbnode *u,struct rbnode *v);
void rb_delete_fixup(RBtree *T,struct rbnode *x);

struct rbnode *rbtree_add(RBtree *root,uint64_t key, char value);
struct rbnode *rbtree_lookup(struct rbnode *node,struct rbnode *nil, uint64_t key);
struct rbnode *rbtree_delete(RBtree *T, uint64_t key);
struct rbnode *rbtree_min(struct rbnode *node,struct rbnode *nil);
struct rbnode *rbtree_max(struct rbnode *node,struct rbnode *nil);
void rbtree_free(RBtree *T);
void recursion_free(struct rbnode *root,struct rbnode *nil);
void rbtree_print_dfs(struct rbnode *root, struct rbnode *nil, int level);

int rbtree_height(struct rbnode *node, struct rbnode *nil);

#endif
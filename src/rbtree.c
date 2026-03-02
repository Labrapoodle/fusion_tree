#include <rbtree.h>
#include <stdlib.h>
#include <stdio.h>

RBtree *rbtree_create(){
    RBtree *T = (RBtree*)malloc(sizeof(RBtree));
    T->nil = (struct rbnode*)malloc(sizeof(struct rbnode));
    T->nil->parent = NULL;
    T->nil->left = NULL;
    T->nil->right = NULL;
    T->nil->color = 1;
    T->nil->key = 0;
    T->nil->value = 'O';
    T->root = T->nil;
    return T;
}

struct rbnode *rbtree_add(RBtree *tree,uint64_t key, char value){
    struct rbnode *y = tree->nil;
    struct rbnode *x = tree->root;
    struct rbnode *z = (struct rbnode*)malloc(sizeof(*z));
    if(z==NULL){
        printf("RBFAIL %lu\n",key);
        exit(EXIT_FAILURE);
    }
    z->parent = tree->nil;    
    z->key = key;
    z->value = value;

    while (x != tree->nil){
        y=x;
        if(key<x->key) x = x->left;
        else x = x->right;
    }
    z->parent = y;
    if(y==tree->nil) tree->root = z;
    else if(key<y->key) y->left = z;
    else y->right = z;
    z->left= tree->nil;
    z->right = tree->nil;
    z->color = 0;
    
    rb_add_fixup(tree,z);
    
    return NULL;
}

void rb_add_fixup(RBtree *T,struct rbnode *z){
    struct rbnode *y;
    while (z->parent->color == 0){
        
        if(z->parent == z->parent->parent->left){
            y = z->parent->parent->right;
            if(y->color==0){
                z->parent->color=1;
                y->color=1;
                z->parent->parent->color=0;
                z=z->parent->parent;
            }
            else{ 
                if(z == z->parent->right){
                    z = z->parent;
                    
                    left_rotate(T,z);
                    
                }
                z->parent->color = 1;
                z->parent->parent->color = 0;
                
                right_rotate(T,z->parent->parent);
                
            }
        
        }
        
        else{
            y = z->parent->parent->left;
            if(y->color==0){
                z->parent->color=1;
                y->color=1;
                z->parent->parent->color=0;
                z=z->parent->parent;
            }
            else{ 
                if(z == z->parent->left){
                    z = z->parent;
                    right_rotate(T,z);
                }
                z->parent->color = 1;
                z->parent->parent->color = 0;
                left_rotate(T,z->parent->parent);
            }
        }


    }
    T->root->color=1;
    
}

void left_rotate(RBtree *T,struct rbnode *x){
    struct rbnode *y = x->right;
    x->right = y->left;
    if(y->left != T->nil) y->left->parent = x;
    y->parent = x->parent;
    if(x->parent == T->nil) T->root = y;
    else if(x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}
void right_rotate(RBtree *T,struct  rbnode *x){
    struct rbnode *y = x->left;
    x->left = y->right;
    if(y->right != T->nil) y->right->parent = x;
    y->parent = x->parent;
    if(x->parent == T->nil) T->root = y;
    else if(x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->right = x;
    x->parent = y;
}

struct rbnode *rbtree_lookup(struct rbnode *node,struct rbnode *nil, uint64_t key){
    
    while(node!=nil){
        if(key==node->key) return node;
        else if (key<node->key) node = node->left;
        else node = node->right;
    }
    return node;
}

struct rbnode *rbtree_min(struct rbnode *node,struct rbnode *nil){
    
    if(node==nil) return nil;
    while(node->left != nil) node = node->left;
    return node;
}

struct rbnode *rbtree_max(struct rbnode *node,struct rbnode *nil){
    
    if(node==nil) return nil;
    while(node->right != nil) node = node->right;
    return node;
}

void rbtree_free(RBtree *T){
    recursion_free(T->root,T->nil);
    free(T->nil);
    free(T);

}
void recursion_free(struct rbnode *tree,struct rbnode *nil){
    if(tree == nil) return;
    recursion_free(tree->left,nil);
    recursion_free(tree->right,nil);
    free(tree);
}

struct rbnode *rbtree_delete(RBtree *T, uint64_t key){
    struct rbnode *z = rbtree_lookup(T->root,T->nil,key);
    struct rbnode *x,*y =z;
    char y_orig_color = y->color;
    if(z->left == T->nil){
        x = z->right;
        rbtree_transplant(T,z,z->right);
    }
    else if(z->right == T->nil ){
        x = z->left;
        rbtree_transplant(T,z,z->left);
    }
    else{
        y = rbtree_min(z->right,T->nil);
        y_orig_color = y->color;
        x = y->right;
        if(y->parent == z) x->parent = y;
        else{
            rbtree_transplant(T,y,y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rbtree_transplant(T,z,y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if(y_orig_color==1) rb_delete_fixup(T,x);
    
    free(z);
    return y;
}

void rbtree_transplant(RBtree *T,struct rbnode *u,struct rbnode *v){
    if(u->parent == T->nil) T->root = v;
    else if(u == u->parent->left) u->parent->left = v;
    else u->parent->right =v;
    v->parent = u->parent;
}

void rb_delete_fixup(RBtree *T,struct rbnode *x){
    struct rbnode *w;
    while(x!=T->root && x->color ==1){
        if(x ==x->parent->left){
            w = x->parent->right;
            if(w->color ==0){
                w->color = 1;
                x->parent->color = 0;
                left_rotate(T,x->parent);
                w = x->parent->right;
            }
            if(w->left->color == 1 && w->right->color ==1){
                w->color = 0;
                x = x->parent;
            }
            else{
                if(w->right->color == 1){
                    w->left->color = 1;
                    w->color = 0;
                    right_rotate(T,w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = 1;
                w->right->color = 1;
                left_rotate(T,x->parent);
                x = T->root;
            }
        }
        else{
            w = x->parent->left;
            if(w->color ==0){
                w->color = 1;
                x->parent->color = 0;
                right_rotate(T,x->parent);
                w = x->parent->left;
            }
            if(w->right->color == 1 && w->left->color ==1){
                w->color = 0;
                x = x->parent;
            }
            else{
                if(w->left->color == 1){
                    w->right->color = 1;
                    w->color = 0;
                    left_rotate(T,w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 1;
                w->left->color = 1;
                right_rotate(T,x->parent);
                x = T->root;
            }
        }

    }
    x->color=1;
}

void rbtree_print_dfs(struct rbnode *root, struct rbnode *nil, int level){
    struct rbnode *node = root;
    
    if (node == nil) return;
    for (int i = 0; i < level; i++) printf("   ");
    printf("%lu(%c)\n", node->key, node->color == 0 ? 'R' : 'B');
    rbtree_print_dfs(node->right, nil, level + 1);
    
    
    rbtree_print_dfs(node->left, nil, level + 1);
    
}

int rbtree_height(struct rbnode *node, struct rbnode *nil) {
    if (node == nil) return 0;
    int left_height = rbtree_height(node->left, nil);
    int right_height = rbtree_height(node->right, nil);
    return 1 + (left_height > right_height ? left_height : right_height);
}
#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
    // rbtree 구조체 1개 크기만큼 메모리를 0으로 초기화하여 할당 - calloc 사용
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
    if (p == NULL) { // 메모리 부족 등으로 실패했을 경우
        return NULL;
    }
#ifdef SENTINEL
    // 모든 빈 자리를 없음을 가리키는 공용 노드 nil를 가리키게 함
    node_t *nil = (node_t *)calloc(1, sizeof(node_t));
    if (nil == NULL) { // nil 생성 실패 시
        free(p);
        return NULL;
    }

    // rbtree에서 모든 nil은 모두 BLACK
    nil->color = RBTREE_BLACK;

    // NIL의 좌/우/부모를 자기 자신
    nil->left = nil;
    nil->right = nil;
    nil->parent = nil;

    p->nil = nil;  // tree의 공용 nil 포인터
    p->root = nil; // 루트 또한 nil
#else
    // SENTINEL 방식이 아닐 경우
    p->root = NULL;
#endif
    return p;
}

void delete_rbtree(rbtree *t) {
    if (t == NULL)
        return;

#ifdef SENTINEL
    free(t->nil);
#endif
    free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
    // TODO: implement insert
    return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
    // TODO: implement find
    return t->root;
}

node_t *rbtree_min(const rbtree *t) {
    // TODO: implement find
    return t->root;
}

node_t *rbtree_max(const rbtree *t) {
    // TODO: implement find
    return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
    // TODO: implement erase
    return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
    // TODO: implement to_array
    return 0;
}

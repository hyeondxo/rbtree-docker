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

/**
 * sentinel 방식만을 사용하는 구현
 * 상황 가정
 * x의 부모가 p
 * x의 right(오른쪽 자식)이 y, left가 A
 * y의 왼쪽 자식이 B, 오른쪽 자식이 C
 * x의 오른쪽 자식 y를 x 자리로 끌어올리고, x를 y의 왼쪽 자식으로 보내는 좌회전
 */
static void left_rotate(rbtree *t, node_t *x) {
    node_t *y = x->right;    // 1. y는 x의 오른쪽 자식
    x->right = y->left;      // 2. x의 오른쪽 자식을 y가 아닌 y의 왼쪽 자식인 B로 변경
    if (y->left != t->nil) { // 3. 만약 y의 왼쪽 자식 B가 존재한다면 (nil이 아니라면)
        y->left->parent = x; // 4. y 왼쪽 자식 B의 부모는 x가 됨
    }

    y->parent = x->parent;             // 5. y의 부모를 x의 부모로 변경
    if (x->parent == t->nil) {         // 6. 만약 x 자신이 트리의 루트라면 ()
        t->root = y;                   // 7. 트리의 루트는 y가 됨
    } else if (x == x->parent->left) { // 8. 만약 x의 부모의 left가 x라면 (x가 왼쪽 자식이라면)
        x->parent->left = y;           // 9. 기존 x 부모의 왼쪽 자식을 y로 변경
    } else {                           // 10. x가 부모의 오른쪽 자식이었다면
        x->parent->right = y;          // 11. x 부모 오른쪽이 y가 됨
    }

    y->left = x;   // 12. y의 왼쪽 자식을 x로 변경
    x->parent = y; // 13. x의 부모를 y로 변경
}

/**
 * sentinel 방식만을 사용하는 구현 (우회전)
 * 상황 가정
 * y의 부모가 p
 * y의 left(왼쪽 자식)이 x, right가 C
 * x의 왼쪽 자식이 A, 오른쪽 자식이 B
 * y의 왼쪽 자식 x를 y 자리로 끌어올리고, y를 x의 오른쪽 자식으로 보내는 우회전
 */
static void right_rotate(rbtree *t, node_t *y) {
    node_t *x = y->left;      // 1. x는 y의 왼쪽 자식
    y->left = x->right;       // 2. y의 왼쪽 자식을 x의 오른쪽 자식 B로 변경
    if (x->right != t->nil)   // 3. 만약 x의 오른쪽 자식 B가 존재한다면 (nil이 아니라면)
        x->right->parent = y; // 4. B의 부모는 y가 됨

    x->parent = y->parent;             // 5. x의 부모를 y의 부모로 변경 (x가 y의 자리로 승격)
    if (y->parent == t->nil) {         // 6. y 자신이 트리의 루트라면
        t->root = x;                   // 7. 트리의 루트는 x가 됨
    } else if (y == y->parent->left) { // 8. y가 부모의 왼쪽 자식이었다면
        y->parent->left = x;           // 9. 부모의 왼쪽 자식을 x로 변경
    } else {                           // 10. y가 부모의 오른쪽 자식이었다면
        y->parent->right = x;          // 11. 부모의 오른쪽 자식을 x로 변경
    }

    x->right = y;  // 12. x의 오른쪽 자식을 y로 변경
    y->parent = x; // 13. y의 부모를 x로 변경
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

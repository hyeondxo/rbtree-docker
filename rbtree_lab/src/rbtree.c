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

static void free_subtree(rbtree *t, node_t *x) {
    if (x == t->nil) {
        return;
    }
    // 후위 순회를 통해 모든 자식 노드까지 메모리 해제
    // -> 왜 후위순회여야 하는가?
    // 부모를 먼저 해제한다면 자식 노드를 참조할 수 없게 되기때문에 자식을 해제하지 못함
    free_subtree(t, x->left);
    free_subtree(t, x->right);
    free(x);
}

void delete_rbtree(rbtree *t) {
    if (t == NULL)
        return;
    free_subtree(t, t->root);
    free(t->nil); // 센티넬 해제
    free(t);      // 트리 자체를 해제
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

// 삽입 이후 rbtree가 규칙을 위반하지 않도록 복구
static void rbtree_fixup(rbtree *t, node_t *z) {
    // 삽입된 노드의 부모가 RED인 경우만 반복. 부모가 BLACK이어야 규칙 4를 위반x
    while (z->parent->color == RBTREE_RED) {
        if (z->parent == z->parent->parent->left) { // z의 부모가 z의 조부모의 왼쪽 자식이라면
            node_t *y = z->parent->parent->right;   // 삼촌을 확인해야 함
            // Case 1 : z 삼촌의 색이 RED라면 -> 부모, 삼촌 둘 다 RED
            if (y->color == RBTREE_RED) {
                // z 조부모와 그 자식들의 색을 바꾼 후 문제를 위로 올림
                z->parent->color = RBTREE_BLACK;       // 1. z 부모의 색 BLACK
                y->color = RBTREE_BLACK;               // 2. z 삼촌의 색 BLACK
                z->parent->parent->color = RBTREE_RED; // 3. z 조부모의 색 RED
                z = z->parent->parent;                 // 4. 조부모부터 다시 검사
            } else {                                   // z 삼촌의 색이 BLACK
                // Case 2 : z가 zp의 오른쪽 자식일 경우
                if (z == z->parent->right) {
                    z = z->parent; // 회전 축을 z에서 z의 부모로 변경
                    // 윗줄이 잘 이해가 안갔음. left rotate시 z의 부모가 원래 z가 있어야 할 자리로 가기 때문에
                    // z를 z->parent로 변경해주어야 올바른 case 3으로 시작할 수 있게 되는 거였다.
                    left_rotate(t, z); // 좌회전하여 case 3으로 만들기
                }
                // Case 3 : z가 z 부모의 왼쪽 자식
                z->parent->color = RBTREE_BLACK;       // 1. z의 부모를 black
                z->parent->parent->color = RBTREE_RED; // 2. z의 조부모를 red
                right_rotate(t, z->parent->parent);    // 3. z의 조부모를 기준으로 right rotate
                // -> RED-RED 인접이 해소되고 Black-height가 유지됨
            }
        } else {                                 // 대칭 케이스 : 부모가 조부모의 오른쪽 자식
            node_t *y = z->parent->parent->left; // 삼촌은 조부모의 왼쪽
            // Case 1
            if (y->color == RBTREE_RED) {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            } else {
                // Case 2
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(t, z);
                }
                // Case 3
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                left_rotate(t, z->parent->parent);
            }
        }
    }
    t->root->color = RBTREE_BLACK; // 루트의 색은 항상 BLACK
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
    node_t *z = (node_t *)malloc(sizeof(node_t)); // key를 넣을 새 노드 z 동적 할당
    if (z == NULL) {                              // z 메모리 할당 실패
        return NULL;
    }
    // z 노드 초기화
    z->key = key;
    z->color = RBTREE_RED;
    z->parent = z->left = z->right = t->nil;

    node_t *x = t->root; // x가 루트부터 내려갈 노드
    node_t *y = t->nil;  // z의 부모 후보
    // z의 올바른 삽입 위치 찾기
    while (x != t->nil) {
        y = x;
        if (key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    // z와 부모 자식 연결
    z->parent = y;
    if (y == t->nil) {
        t->root = z;
    } else if (key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    // rb트리의 조건을 모두 만족하도록 복구
    rbtree_fixup(t, z);

    return z; // 삽입된 노드의 포인터 반환
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
    node_t *x = t->root;

    while (x != t->nil) {
        if (x->key == key) {
            return x;
        }
        if (key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    return NULL;
}

// 트리에서의 최솟값 반환
node_t *rbtree_min(const rbtree *t) {
    node_t *x = t->root;
    if (x == NULL) {
        return NULL;
    }
    while (x->left != t->nil) {
        x = x->left;
    }
    return x;
}

// 트리의 최댓값 반환
node_t *rbtree_max(const rbtree *t) {
    node_t *x = t->root;
    if (x == NULL) {
        return NULL;
    }
    while (x->right != t->nil) {
        x = x->right;
    }
    return x;
}

// 서브트리에서 successor 찾기 (오른쪽 서브트리 중 가장 작은 값)
static node_t *subtree_min(rbtree *t, node_t *x) {
    while (x->left != t->nil) { // 왼쪽 끝까지 내려가며 최소 찾기
        x = x->left;
    }
    return x;
}

// u의 자리에 v를 이식
static void transplant(rbtree *t, node_t *u, node_t *v) {
    if (u->parent == t->nil) { // u가 루트였다면
        t->root = v;
    } else if (u == u->parent->left) { // u가 왼쪽 자식이었다면
        u->parent->left = v;
    } else { // u가 오른쪽 자식이었다면
        u->parent->right = v;
    }
    v->parent = u->parent; // 부모 갱신
}

// 삭제 후 doubly black을 해소
// x는 검정 높이를 보전해야하는 자리
static void delete_fixup(rbtree *t, node_t *x) {
    // x가 루트가 아니고, x가 검정일 때만 반복
    while (x != t->root && x->color == RBTREE_BLACK) {
        if (x == x->parent->left) {       // x가 왼쪽 자식일 경우
            node_t *w = x->parent->right; // x의 형제 w
            // Case 1 : 형제가 RED
            if (w->color == RBTREE_RED) {
                w->color = RBTREE_BLACK;       // 형제를 BLACK
                x->parent->color = RBTREE_RED; // 부모를 RED
                left_rotate(t, x->parent);     // 부모 기준 좌회전으로 검정 형제의 상황 만들기
                w = x->parent->right;          // 새로운 형제 갱신
            }

            // 여기부터는 형제가 BLACK
            // Case 2 : 형제의 두 자식 모두 BLACK -> 형제를 RED로 칠하고 부모로 extra-black을 올려보냄
            if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
                w->color = RBTREE_RED;
                x = x->parent;
            } else {
                if (w->right->color == RBTREE_BLACK) {
                    // Case 3 : 형제의 오른쪽 자식이 BLACK, 왼쪽 자식이 RED
                    w->left->color = RBTREE_BLACK; // 왼쪽 자식을 BLACK
                    w->color = RBTREE_RED;         // 형제는 RED
                    right_rotate(t, w);            // 형제 기준 우회전으로 Case 4를 만들고 Case 4로 해결
                    w = x->parent->right;          // 형제 갱신
                }
                // Case 4 : 형제의 오른쪽 자식이 RED
                w->color = x->parent->color;     // 형제는 부모의 색을 물려받음
                x->parent->color = RBTREE_BLACK; // 부모는 BLACK
                w->right->color = RBTREE_BLACK;  // 형제의 오른쪽 자식을 BLACK
                left_rotate(t, x->parent);       // 부모 기준 좌회전
                x = t->root; // 이거 왜하냐 -> while문 종료의 break의 역할임. Case 4가 해결되면 무조건 해결됨
            }
        } else { // 대칭 : x가 오른쪽 자식인 경우
            node_t *w = x->parent->left;
            // Case 1
            if (w->color == RBTREE_RED) {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotate(t, x->parent);
                w = x->parent->left;
            }
            // Case 2
            if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
                w->color = RBTREE_RED;
                x = x->parent;
            } else {
                // Case 3
                if (w->left->color == RBTREE_BLACK) {
                    w->right->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    left_rotate(t, w);
                    w = x->parent->left;
                }
                // Case 4
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->left->color = RBTREE_BLACK;
                right_rotate(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = RBTREE_BLACK; // x는 삭제 노드를 대체하게된 노드. 이것을 BLACK으로 설정하여 규칙 2, 4를 해결
}

int rbtree_erase(rbtree *t, node_t *z) {
    if (t == NULL || z == NULL || z == t->nil) {
        return -1;
    }

    node_t *y = z;                     // 트리에서 제거될 노드
    color_t y_origin_color = y->color; // 제거되는 노드의 원래 색
    node_t *x;                         // y를 치환하고 남는 자리

    if (z->left == t->nil) {         // 왼쪽 자식이 없는 경우
        x = z->right;                // z 자리를 z->right로 매움
        transplant(t, z, z->right);  // z 위치에 z의 오른쪽 자식을 이식
    } else if (z->right == t->nil) { // 오른쪽 자식이 없는 경우
        x = z->left;
        transplant(t, z, z->left);
    } else {                          // 자식이 둘 다 있는 경우
        y = subtree_min(t, z->right); // 후계자 찾기
        y_origin_color = y->color;    // 기존 색깔 저장
        x = y->right;                 // x가 삭제된 노드의 대체가 되기때문에 y->right로하면 nil이나
        if (y->parent == z) {         // y의 부모가 z라면 -> 바로 오른쪽 자식이 최소
            x->parent = y;
        } else {
            transplant(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(t, z, y); // y가 대체되었으니 왼쪽 오른쪽을 올바르게 이어주기
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    free(z);
    if (y_origin_color == RBTREE_BLACK) {
        delete_fixup(t, x);
    }

    return 0;
}

// rbtree를 중위 순회하며 결과 배열에 저장
// t, x는 읽기만 하므로 const가 적절
static void inorder_store(const rbtree *t, const node_t *x, key_t *result, size_t *index, const size_t n) {
    if (x == t->nil) {
        return;
    }
    // 여기서 index의 타입은 이미 size_t * 이다
    inorder_store(t, x->left, result, index, n);
    if (*index < n) { // 유효한 범위(n)만큼만 결과 삽입. 테스트에서 필수는 아님
        result[*index] = x->key;
        (*index)++;
    } else {
        return;
    }
    inorder_store(t, x->right, result, index, n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
    if (t == NULL || arr == NULL) {
        return -1;
    }
    size_t index = 0;
    /**
     * int가 아닌 size_t 타입을 왜 쓰는가?
     * size_t는 크기나 인덱스를 표현하는데 최적화된 타입임
     * C 표준 라이브러리에서 malloc, sizeof, strlen과 같은 함수들이 반환하는 값이 전부 size_t
     * unsigned int와 유사하게 양수만 표현이 가능하지만 더 큰 범위를 가짐
     * -> 메모리나 배열 크기를 표현할 때 절대 음수가 될 수 없고, 시스템 메모리 한계까지 표현 가능
     */
    /**
     * index를 값 전달이 아닌 주소 전달을 해야하는 이유?
     * index를 값 전달로 사용하게 되면 각 재귀 호출마다 index 값이 복사되어 사용됨
     * 즉, 왼쪽 서브트리에 값을 넣고 돌아왔을 때 index 값이 갱신되지 않고 계속 0번째 칸에 덮어씌워짐
     * index의 주소를 넘기면 모든 재귀에서 하나의 index를 공유할 수 있음 -> 증가된 값 유지
     * 따라서 왼쪽 서브트리에 몇 개를 넣었는지, result 배열에 얼만큼 채워졌는지를 알 수 있기 때문에 올바른 누적이 가능
     * -> 오름차순 정렬 배열을 구현할 수 있게됨
     */
    inorder_store(t, t->root, arr, &index, n);
    return 0;
}

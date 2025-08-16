#include <assert.h>
#include <rbtree.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// new_rbtree should return rbtree struct with null root node
// 새로 만든 Red-Black Tree가 빈 트리 상태로 올바르게 초기화되었는지 검증
void test_init(void) {
    rbtree *t = new_rbtree();  // 실제 트리 구조체를 동적 할당, 내부 필드를 기본값으로 세팅(초기화)
    assert(t != NULL);         // t가 null을 반환하였는지 검사 -> 위 작업이 성공인지 실패인지
#ifdef SENTINEL                // 감시(sentinel) 노드를 사용하였는지 검사하는 조건 분기
    assert(t->nil != NULL);    // sentinel 노드에서 nil이 존재하는지 검사
    assert(t->root == t->nil); // root가 nil을 가리키는지
#else
    assert(t->root == NULL); // sentinel을 사용중이지 않다면 root는 null
#endif
    delete_rbtree(t); // 트리 전체 메모리를 해제. sentinel 방식이라면 t->nil까지 모두 해제
}

// root node should have proper values and pointers
void test_insert_single(const key_t key) {
    rbtree *t = new_rbtree();          // 빈 rbtree를 생성
    node_t *p = rbtree_insert(t, key); // key를 삽입 후 삽입된 노드의 포인터를 반환
    assert(p != NULL);                 // 삽입된 결과는 유효한 포인터여야 함
    assert(t->root == p);              // 첫 삽입이므로 방금 삽입 노드가 루트여야 함
    assert(p->key == key);             // 루트의 key값이 방금 삽입한 key와 같아야 함
    // assert(p->color == RBTREE_BLACK);  // color of root node should be black
#ifdef SENTINEL
    // root의 왼쪽, 오른쪽, 부모는 sentinel
    assert(p->left == t->nil);
    assert(p->right == t->nil);
    assert(p->parent == t->nil);
#else
    assert(p->left == NULL);
    assert(p->right == NULL);
    assert(p->parent == NULL);
#endif
    delete_rbtree(t);
}

// find should return the node with the key or NULL if no such node exists
void test_find_single(const key_t key, const key_t wrong_key) {
    rbtree *t = new_rbtree();          // 빈 rbtree 생성
    node_t *p = rbtree_insert(t, key); // 노드 삽입 후 포인터 반환

    node_t *q = rbtree_find(t, key); // 방금 삽입한 key를 탐색
    assert(q != NULL);               // 찾았으니 null이면 안됨
    assert(q->key == key);           // 찾은 노드의 key와 일치해야함
    assert(q == p);                  // 주소까지 동일해야 함

    q = rbtree_find(t, wrong_key); // 존재하지 않는 key로 검색
    assert(q == NULL);             // 없을테니 NULL 반환

    delete_rbtree(t);
}

// erase should delete root node
void test_erase_root(const key_t key) {
    rbtree *t = new_rbtree();
    node_t *p = rbtree_insert(t, key);
    assert(p != NULL);
    assert(t->root == p);
    assert(p->key == key);

    rbtree_erase(t, p);
#ifdef SENTINEL
    assert(t->root == t->nil);
#else
    assert(t->root == NULL);
#endif

    delete_rbtree(t);
}

static void insert_arr(rbtree *t, const key_t *arr, const size_t n) {
    for (size_t i = 0; i < n; i++) {
        rbtree_insert(t, arr[i]);
    }
}

static int comp(const void *p1, const void *p2) {
    const key_t *e1 = (const key_t *)p1;
    const key_t *e2 = (const key_t *)p2;
    if (*e1 < *e2) {
        return -1;
    } else if (*e1 > *e2) {
        return 1;
    } else {
        return 0;
    }
};

// min/max should return the min/max value of the tree
void test_minmax(key_t *arr, const size_t n) {
    // null array is not allowed
    assert(n > 0 && arr != NULL);

    rbtree *t = new_rbtree();
    assert(t != NULL);

    insert_arr(t, arr, n);
    assert(t->root != NULL);
#ifdef SENTINEL
    assert(t->root != t->nil);
#endif

    qsort((void *)arr, n, sizeof(key_t), comp);
    node_t *p = rbtree_min(t);
    assert(p != NULL);
    assert(p->key == arr[0]);

    node_t *q = rbtree_max(t);
    assert(q != NULL);
    assert(q->key == arr[n - 1]);

    rbtree_erase(t, p);
    p = rbtree_min(t);
    assert(p != NULL);
    assert(p->key == arr[1]);

    if (n >= 2) {
        rbtree_erase(t, q);
        q = rbtree_max(t);
        assert(q != NULL);
        assert(q->key == arr[n - 2]);
    }

    delete_rbtree(t);
}

/**
 * t : rbtree 포인터
 * arr : 트리에 삽입할 값들 (배열)
 * n : 배열 원소 개수
 */
void test_to_array(rbtree *t, const key_t *arr, const size_t n) {
    assert(t != NULL);

    insert_arr(t, arr, n);                      // 트리에 arr 삽입
    qsort((void *)arr, n, sizeof(key_t), comp); // arr을 오름차순 정렬

    key_t *res = calloc(n, sizeof(key_t)); // 결과 배열 동적 할당. 배열 원소 개수 n * key_t(int)의 size
    rbtree_to_array(t, res, n);            // 트리의 중위순회 결과를 res에 채워넣음 -> 오름차순
    for (int i = 0; i < n; i++) {
        assert(arr[i] == res[i]); // 원본 배열의 오름차순 결과와 트리에서 중위 순회를 통해 가져온 결과가 같아야 함
    }
    free(res);
}

// 트리의 인스턴스 2개를 동시에 운영해도 각각 정렬/불변이 유지되는지 검증
void test_multi_instance() {
    rbtree *t1 = new_rbtree();
    assert(t1 != NULL);
    rbtree *t2 = new_rbtree();
    assert(t2 != NULL);

    key_t arr1[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
    const size_t n1 = sizeof(arr1) / sizeof(arr1[0]);
    insert_arr(t1, arr1, n1);
    qsort((void *)arr1, n1, sizeof(key_t), comp);

    key_t arr2[] = {4, 8, 10, 5, 3};
    const size_t n2 = sizeof(arr2) / sizeof(arr2[0]);
    insert_arr(t2, arr2, n2);
    qsort((void *)arr2, n2, sizeof(key_t), comp);

    key_t *res1 = calloc(n1, sizeof(key_t));
    rbtree_to_array(t1, res1, n1);
    for (int i = 0; i < n1; i++) {
        assert(arr1[i] == res1[i]);
    }

    key_t *res2 = calloc(n2, sizeof(key_t));
    rbtree_to_array(t2, res2, n2);
    for (int i = 0; i < n2; i++) {
        assert(arr2[i] == res2[i]);
    }

    free(res2);
    free(res1);
    delete_rbtree(t2);
    delete_rbtree(t1);
}

// Search tree constraint
// The values of left subtree should be less than or equal to the current node
// The values of right subtree should be greater than or equal to the current
// node

static bool search_traverse(const node_t *p, key_t *min, key_t *max, node_t *nil) {
    if (p == nil) {
        return true;
    }

    *min = *max = p->key;

    key_t l_min, l_max, r_min, r_max;
    l_min = l_max = r_min = r_max = p->key;

    const bool lr = search_traverse(p->left, &l_min, &l_max, nil);
    if (!lr || l_max > p->key) {
        return false;
    }
    const bool rr = search_traverse(p->right, &r_min, &r_max, nil);
    if (!rr || r_min < p->key) {
        return false;
    }

    *min = l_min;
    *max = r_max;
    return true;
}

void test_search_constraint(const rbtree *t) {
    assert(t != NULL);
    node_t *p = t->root;
    key_t min, max;
#ifdef SENTINEL
    node_t *nil = t->nil;
#else
    node_t *nil = NULL;
#endif
    assert(search_traverse(p, &min, &max, nil));
}

// Color constraint
// 1. Each node is either red or black. (by definition)
// 2. All NIL nodes are considered black.
// 3. A red node does not have a red child.
// 4. Every path from a given node to any of its descendant NIL nodes goes
// through the same number of black nodes.

bool touch_nil = false;
int max_black_depth = 0;

static void init_color_traverse(void) {
    touch_nil = false;
    max_black_depth = 0;
}

static bool color_traverse(const node_t *p, const color_t parent_color, const int black_depth, node_t *nil) {
    if (p == nil) {
        if (!touch_nil) {
            touch_nil = true;
            max_black_depth = black_depth;
        } else if (black_depth != max_black_depth) {
            return false;
        }
        return true;
    }
    if (parent_color == RBTREE_RED && p->color == RBTREE_RED) {
        return false;
    }
    int next_depth = ((p->color == RBTREE_BLACK) ? 1 : 0) + black_depth;
    return color_traverse(p->left, p->color, next_depth, nil) && color_traverse(p->right, p->color, next_depth, nil);
}

void test_color_constraint(const rbtree *t) {
    assert(t != NULL);
#ifdef SENTINEL
    node_t *nil = t->nil;
#else
    node_t *nil = NULL;
#endif
    node_t *p = t->root;
    assert(p == nil || p->color == RBTREE_BLACK);

    init_color_traverse();
    assert(color_traverse(p, RBTREE_BLACK, 0, nil));
}

// rbtree should keep search tree and color constraints
void test_rb_constraints(const key_t arr[], const size_t n) {
    rbtree *t = new_rbtree();
    assert(t != NULL);

    insert_arr(t, arr, n);
    assert(t->root != NULL);

    test_color_constraint(t);
    test_search_constraint(t);

    delete_rbtree(t);
}

// rbtree should manage distinct values
// 삭제 기능 사용 x, 중복 없는 값들에 대해 RED-BLACK 불변식과 BST 불변식 유지를 검증
void test_distinct_values() {
    const key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
    const size_t n = sizeof(entries) / sizeof(entries[0]);
    test_rb_constraints(entries, n);
}

// rbtree should manage values with duplicate
// 중복 값 존재 시에도 RED-BLACK, 탐색 불변식 유지를 검증
void test_duplicate_values() {
    const key_t entries[] = {10, 5, 5, 34, 6, 23, 12, 12, 6, 12};
    const size_t n = sizeof(entries) / sizeof(entries[0]);
    test_rb_constraints(entries, n);
}

void test_minmax_suite() {
    key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
    const size_t n = sizeof(entries) / sizeof(entries[0]);
    test_minmax(entries, n);
}

// 중위순회 결과가 오름차순 배열과 같은지 검증
void test_to_array_suite() {
    rbtree *t = new_rbtree();
    assert(t != NULL);

    key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
    const size_t n = sizeof(entries) / sizeof(entries[0]);
    test_to_array(t, entries, n);

    delete_rbtree(t);
}

void test_find_erase(rbtree *t, const key_t *arr, const size_t n) {
    for (int i = 0; i < n; i++) {
        node_t *p = rbtree_insert(t, arr[i]);
        assert(p != NULL);
    }

    for (int i = 0; i < n; i++) {
        node_t *p = rbtree_find(t, arr[i]);
        // printf("arr[%d] = %d\n", i, arr[i]);
        assert(p != NULL);
        assert(p->key == arr[i]);
        rbtree_erase(t, p);
    }

    for (int i = 0; i < n; i++) {
        node_t *p = rbtree_find(t, arr[i]);
        assert(p == NULL);
    }

    for (int i = 0; i < n; i++) {
        node_t *p = rbtree_insert(t, arr[i]);
        assert(p != NULL);
        node_t *q = rbtree_find(t, arr[i]);
        assert(q != NULL);
        assert(q->key == arr[i]);
        assert(p == q);
        rbtree_erase(t, p);
        q = rbtree_find(t, arr[i]);
        assert(q == NULL);
    }
}

void test_find_erase_fixed() {
    const key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12, 24, 36, 990, 25};
    const size_t n = sizeof(arr) / sizeof(arr[0]);
    rbtree *t = new_rbtree();
    assert(t != NULL);

    test_find_erase(t, arr, n);

    delete_rbtree(t);
}

void test_find_erase_rand(const size_t n, const unsigned int seed) {
    srand(seed);
    rbtree *t = new_rbtree();
    key_t *arr = calloc(n, sizeof(key_t));
    for (int i = 0; i < n; i++) {
        arr[i] = rand();
    }

    test_find_erase(t, arr, n);

    free(arr);
    delete_rbtree(t);
}

int main(void) {
    test_init();
    test_insert_single(1024);
    test_find_single(512, 1024);
    // test_erase_root(128);
    // test_find_erase_fixed();
    // test_minmax_suite();
    test_to_array_suite();
    test_distinct_values();
    test_duplicate_values();
    test_multi_instance();
    // test_find_erase_rand(10000, 17);
    printf("Passed all tests!\n");
}

#include "rbtree.h"

#include <stdlib.h>

void left_rotate(rbtree *, node_t *);
void right_rotate(rbtree *, node_t *);
void rbtree_insertfixup(rbtree *, node_t *);
void rb_transplant(rbtree *, node_t *, node_t *);
// void rbtree_erase_fixup(rbtree *, node_t *);
int node_to_array(const rbtree *, node_t *, key_t *, int, int);
node_t *tree_min(const rbtree *, node_t *);
node_t *successor(rbtree *t, node_t *cur);

rbtree *new_rbtree(void) { //동적 메모리 할당...새로운 트리의 생성
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  node_t *NIL = (node_t *)calloc(1, sizeof(node_t));//sentinel: 경계노드
  NIL->color = RBTREE_BLACK;
  p->nil = NIL; //자식은 NIL
  p->root = NIL; //부모도 NIL

  return p; //새롭게 만들어진 트리
}

void delete_node(rbtree *t, node_t *n) { //할당되었던 메모리들을 모조리 삭제한다.
  // if(n == t->nil) {
  //   return;
  // }
  if (n->left != t->nil)
    delete_node(t, n->left);
  if (n->right != t->nil)
    delete_node(t, n->right);
  free(n);
}

void delete_rbtree(rbtree *t) {
  //valgrind로 탐색 시 메모리 미수거가 남지 않아야 함
  // TODO: reclaim the tree nodes's memory
  if(t->root != t->nil)
    delete_node(t, t->root);
  free(t->nil);
  free(t);
}

void left_rotate(rbtree *t, node_t *x) {
  node_t *y;
  y = x->right; // rotate할 노드
  
  x->right = y->left;
  // sentinel이 아니라면 바꾸어주는 노드의 부모를 바꿔줌
  if (y->left != t->nil) {
    y->left->parent = x;
  }

  // 부모노드를 서로 바꾸어 줌
  y->parent = x->parent; 
  
  //x가 left, right, root인지에 따라 부모시점에서의 위치를 y로 바꿈
  if (x->parent == t->nil) { 
     t->root = y;
  }
  else if (x == x->parent->left) {
    x->parent->left = y;
  }
  else {
    x->parent->right = y;
  }
  
  y->left = x;
  
  x->parent = y;
}

void right_rotate(rbtree *t, node_t *x) {
  node_t *y;
  y = x->left; // rotate할 노드
  
  x->left = y->right;
  // sentinel이 아니라면 바꾸어주는 노드의 부모를 바꿔줌
  if (y->right != t->nil) {
    y->right->parent = x;
  }

  // 부모노드를 서로 바꾸어 줌
  y->parent = x->parent; 
  
  //x가 left, right, root인지에 따라 부모시점에서의 위치를 y로 바꿈
  if (x->parent == t->nil) { 
     t->root = y;
  }
  else if (x == x->parent->right) {
    x->parent->right = y;
  }
  else {
    x->parent->left = y;
  }
  
  y->right = x;
  
  x->parent = y;

}

void rbtree_insertfixup(rbtree *t, node_t *z) {
  node_t *y;
  while (z->parent->color == RBTREE_RED) { //추가된 노드가 붉으므로 부모는 검정이어야 함
    
    if (z->parent == z->parent->parent->left) {
      y = z->parent->parent->right;
      if (y->color == RBTREE_RED) { // 경우 1
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else if (z == z->parent->right) { //경우 2
        z =z->parent;
        left_rotate(t, z);
      }
      z->parent->color = RBTREE_BLACK; // 경우 3
      z->parent->parent->color = RBTREE_RED;
    }

    else { // 대칭
      y = z->parent->parent->left;
      if (y->color == RBTREE_RED) { // 경우 1
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else if (z == z->parent->left) { //경우 2
        z =z->parent;
        right_rotate(t, z);
      }
      z->parent->color = RBTREE_BLACK; // 경우 3
      z->parent->parent->color = RBTREE_RED;
    }

  }
  t->root->color = RBTREE_BLACK;

}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  //RB-Insert==========================================
  //새로운 노드 생성
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  z->key = key;

  node_t *y, *x;
  y = t->nil; //말단
  x = t->root; //루트

  while (x != t->nil) { // 트리 말단까지 내려간다
    y = x;//while이 끝나면 y는 삽입하게 되는 노드의 부모노드로 남게된다.
    if (z->key < x->key) { //입력하는 노드의 키가 x의 키보다 작다면
      x = x->left;
    }
    else {
      x = x->right;
    }
  }

  z->parent = y;

  if (y == t->nil) { //탐색이 끝났을 때 부모노드가 nil인 경우(아무것도 없는 경우)
    t->root = z;
  }
  else if (z->key < y->key) {
    y->left = z;
  }
  else {
    y->right = z;
  }

  z->color = RBTREE_RED;
  z->left = t->nil; //자식 노드는 모두 센티넬로 연결
  z->right = t->nil;
  //===================================================
  //RB Tree의 규정에 맞지 않을 수 있으므로 이를 맞춰준다.ㄹㅇ

  rbtree_insertfixup(t, z); //left, right rotation포함

  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *x;
  x = t->root; //탐색 노드

  while (x != t->nil) {
    if (key == x->key) {
      return x;//
    }
    else if (key < x->key) {
      x = x->left;
    }
    else if (key > x->key) {
      x = x->right;
    }
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *x_parent = t->nil; //탐색 노드
  node_t *x_child = t->root;

  while (x_child != t->nil) {
    x_parent = x_child;
    x_child = x_child->left;
  }
  return x_parent; //nil노드를 리턴해서는 안된다.
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *x_parent = t->nil; //탐색 노드
  node_t *x_child = t->root;

  while (x_child != t->nil) {
    x_parent = x_child;
    x_child = x_child->right;
  }
  return x_parent; //nil노드를 리턴해서는 안된다.
}

void rb_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil) { //만약 u가 말단이라면
    t->root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent; 
}

// void rbtree_erase_fixup(rbtree *t, node_t *x) {
//   node_t *w;
//   while (x != t->root && x->color == RBTREE_BLACK) {
//     if (x == x->parent->left) {
//       w = x->parent->right;
//       if (w->color == RBTREE_RED) { //Case 1
//         w->color = RBTREE_BLACK;
//         x->parent->color = RBTREE_RED;
//         left_rotate(t, x->parent);
//         w = x->parent->right;
//       }
//       if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) { // Case 2
//         w->color = RBTREE_RED;
//         x = x->parent;
//       }
//       else if (w->right->color == RBTREE_BLACK) { // Case 3
//         w->left->color = RBTREE_BLACK;
//         w->color = RBTREE_RED;
//         right_rotate(t, w);
//         w = x->parent->right;
//       }
//       w->color = x->parent->color;
//       x->parent->color = RBTREE_BLACK;
//       w->right->color = RBTREE_BLACK;
//       left_rotate(t, x->parent);
//       x = t->root;
//     }
//     //대칭
//     else {
//     if (x == x->parent->right) {
//       w = x->parent->left;
//       if (w->color == RBTREE_RED) { //Case 1
//         w->color = RBTREE_BLACK;
//         x->parent->color = RBTREE_RED;
//         right_rotate(t, x->parent);
//         w = x->parent->left;
//       }
//       if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) { // Case 2
//         w->color = RBTREE_RED;
//         x = x->parent;
//       }
//       else if (w->left->color == RBTREE_BLACK) { // Case 3
//         w->right->color = RBTREE_BLACK;
//         w->color = RBTREE_RED;
//         left_rotate(t, w);
//         w = x->parent->left;
//       }
//       w->color = x->parent->color; // Case 4
//       x->parent->color = RBTREE_BLACK;
//       w->left->color = RBTREE_BLACK;
//       right_rotate(t, x->parent);
//       x = t->root;
//       }
//     }
//   }
// }

// 서브 프로시저: Transplant: 삭제할 노드 z와 z를 대체할 노드 y의 연결관계 변화
// erase_fixup -> double black / red and black 처리 필요, 규칙 위반의 경우 고쳐 줌
// left rotate, right rotate 모두 사용
// 이진 탐색 트리의 구현-> 삭제 노드, 대체 노드의 연결 정보를 수정하거나
// 대체 노드의 키 값을 원래의 삭제할 노드에 복사 후 대체 노드를 대신 삭제시키는 방법이 있다.

node_t *tree_min(const rbtree *t, node_t *z) {
  if (z == NULL) { // 주어진 노드가 null이라면.
    return 0;
  }

  node_t *tmp_parent;
  node_t *tmp_child = z;
  while (tmp_child == t->nil) {
    tmp_parent = tmp_child;
    tmp_child = tmp_child->left;
  }
  return tmp_parent;
}

// int rbtree_erase(rbtree *t, node_t *p) {
//   // TODO: implement erase
//   //리프가 없는 경우
//   node_t *y;
//   node_t *x;
//   y = p; //삭제된 노드 또는 트리에서 이동한 노드
//   color_t y_original_color;
//   y_original_color = y->color;
//   if (p->left == t->nil) {
//     x = p->right;
//     rbtree_transplant(t, p, p->right);
//   } else if (p->right == t->nil) {
//     x = p->left;
//     rbtree_transplant(t, p, p->left);
//   } else {
//     y = tree_min(t, p->right); 
//     y_original_color = y->color;
//     x = y->right;
//     if (y->parent == p) {
//       x->parent = y;
//     } else {
//       rbtree_transplant(t, y, y->right);
//       y->right = p->right;
//       y->right->parent = y;
//     }
//     rbtree_transplant(t, p, y);
//     y->left = p->left;
//     y->left->parent = y;
//     y->color = p->color;
//   }
//   if (y_original_color == RBTREE_BLACK) {
//     rbtree_erase_fixup(t, x);
//   }
//   free(p);
//   return 0;
// }

int rbtree_erase(rbtree *t, node_t *p) {
  //p가 없는 노드이면 삭제 작업 안함
  if (p == NULL) {
    return 0;
  }

  // y : 삭제할 노드, x : y의 원래의 위치로 이동할 노드
  node_t *y = p;
  color_t y_original_color = y->color;
  node_t *x;

  //p가 오른쪽 자식만 가질 경우
  if (p->left == t->nil) {
    x = p->right;
    rb_transplant(t, p, p->right);
  } //p가 왼쪽 자식만 가질 경우
  else if (p->right == t->nil) {
    x = p->left;
    rb_transplant(t, p, p->left);
  } //양쪽 자식 모두 가질 경우
  else {
    //오른쪽 서브트리에서 가장 작은 수 반환
    //successor 를 찾는다.
    y = successor(t,p->right);
    y_original_color = y->color;
    x = y->right;
    if (y->parent == p) {
      x->parent = y;
    }
    else {
      rb_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    rb_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }

  //RB-erase-Fixup------------------------------
  if (y_original_color == RBTREE_BLACK) {
    while (x != t->root && x->color == RBTREE_BLACK){
      if(x == x->parent->left) {
        node_t *w = x->parent->right;
        if(w->color == RBTREE_RED) {
          w->color = RBTREE_BLACK;
          x->parent->color = RBTREE_RED;
          left_rotate(t, x->parent);
          w = x->parent->right;
        }
        if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
          w->color = RBTREE_RED;
          x = x->parent;
        }
        else {
          if (w->right->color == RBTREE_BLACK) {
            w->left->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            right_rotate(t, w);
            w = x->parent->right;
          }
          w->color = x->parent->color;
          x->parent->color = RBTREE_BLACK;
          w->right->color = RBTREE_BLACK;
          left_rotate(t, x->parent);
          x = t->root;
        }
      }
      else {
        node_t *w = x->parent->left;
        if(w->color == RBTREE_RED) {
          w->color = RBTREE_BLACK;
          x->parent->color = RBTREE_RED;
          right_rotate(t, x->parent);
          w = x->parent->left;
        }
        if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
          w->color = RBTREE_RED;
          x = x->parent;
        }
        else {
          if (w->left->color == RBTREE_BLACK) {
            w->right->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            left_rotate(t, w);
            w = x->parent->left;
          }
          w->color = x->parent->color;
          x->parent->color = RBTREE_BLACK;
          w->left->color = RBTREE_BLACK;
          right_rotate(t, x->parent);
          x = t->root;
        }
      }
    }
    x->color = RBTREE_BLACK;
  }
  free(p);
  return 0;
}

node_t *successor(rbtree *t, node_t *cur) {
  node_t *n = cur;
  while(n->left != t->nil) {
    n = n->left;
  }
  return n;
}

int node_to_array(const rbtree *t, node_t *node, key_t *arr, int a, int i) {
  if (node == t->nil){
    return i;
  }
  if(i < a) {
    i = node_to_array(t, node->left, arr, a, i);
    arr[i++] = node->key;
    i = node_to_array(t, node->right, arr, a, i);
  }
  return i;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  // rb tree의 내용을 key 순서대로 주어진 array로 변환
  // tree가 더 큰 경우 순서대로 n까지만 반환
  //메모리는 부르는 함수에서 할당
  node_to_array(t, t->root, arr, n, 0);
  return 0;
}


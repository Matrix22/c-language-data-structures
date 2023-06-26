/**
 * @file m_avl.h
 * @author Mihai Negru (determinant289@gmail.com)
 * @version 1.0.0
 * @date 2023-06-12
 *
 * @copyright Copyright (C) 2022-2023 Mihai Negru <determinant289@gmail.com>
 * This file is part of c-language-data-structures.
 *
 * c-language-data-structures is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * c-language-data-structures is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with c-language-data-structures.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MACROS_GENERICS_AVL_BINARY_SEARCH_TREE_UTILS_H_
#define MACROS_GENERICS_AVL_BINARY_SEARCH_TREE_UTILS_H_

#include "./m_config.h"

/**
 * @brief Utility file containing macros that generate functions for generic
 * single linked list.
 *
 * @param ID the id of the structure in order to reduce name collisions.
 * @param T the type of the data stored inside the structure.
 */

#define MAVL_MAX_VALUES(A, B) (((A) >= (B)) ? (A) : (B))

/**
 * @brief Generates the `mavl_t` structure depending on the name and type.
 * Also generates basic function for creation and freeing memory for the
 * structure. This structure require a method for comparing data and for freeing
 * data memory, if data is not stored as a pointer (T <=> *M), then the free
 * function must be `NULL`. If data represents a structure which contains
 * pointers allocated, then the free function must free those fields.
 */
#define MAVL(ID, T)                                                            \
  CMP_FUNC(ID, T)                                                              \
  FREE_FUNC(ID, T)                                                             \
                                                                               \
  typedef struct ID##_mavl_node_s {                                            \
    T data;                                                                    \
    struct ID##_mavl_node_s *parent;                                           \
    struct ID##_mavl_node_s *left;                                             \
    struct ID##_mavl_node_s *right;                                            \
    uint32_t count;                                                            \
    uint32_t height;                                                           \
  } ID##_mavl_node_ptr_t, *ID##_mavl_node_t;                                   \
                                                                               \
  typedef struct ID##_mavl_s {                                                 \
    ID##_mavl_node_t root;                                                     \
    ID##_mavl_node_t nil;                                                      \
    ID##_compare_func cmp;                                                     \
    ID##_free_func frd;                                                        \
    size_t size;                                                               \
  } ID##_mavl_ptr_t, *ID##_mavl_t;                                             \
                                                                               \
  ID##_mavl_t ID##_mavl(ID##_compare_func cmp, ID##_free_func frd) {           \
    if (NULL == cmp) {                                                         \
      return NULL;                                                             \
    }                                                                          \
                                                                               \
    ID##_mavl_t self = malloc(sizeof *self);                                   \
                                                                               \
    if (self == NULL) {                                                        \
      return NULL;                                                             \
    }                                                                          \
                                                                               \
    self->cmp = cmp;                                                           \
    self->frd = frd;                                                           \
                                                                               \
    self->nil = malloc(sizeof *self->nil);                                     \
                                                                               \
    if (self->nil == NULL) {                                                   \
      free(self);                                                              \
      return NULL;                                                             \
    }                                                                          \
                                                                               \
    self->nil->height = 0;                                                     \
    self->nil->count = 1;                                                      \
    self->nil->left = self->nil->right = self->nil->parent = self->nil;        \
                                                                               \
    self->root = self->nil;                                                    \
    self->size = 0;                                                            \
                                                                               \
    return self;                                                               \
  }                                                                            \
                                                                               \
  ID##_mavl_node_t ID##_internal_mavl_node(const ID##_mavl_ptr_t *const self,  \
                                           T *const data) {                    \
    ID##_mavl_node_t self_node = malloc(sizeof *self_node);                    \
                                                                               \
    if (self_node == NULL) {                                                   \
      return self->nil;                                                        \
    }                                                                          \
                                                                               \
    self_node->left = self_node->right = self_node->parent = self->nil;        \
    self_node->count = 1;                                                      \
    self_node->height = 1;                                                     \
    self_node->data = *data;                                                   \
                                                                               \
    return self_node;                                                          \
  }                                                                            \
                                                                               \
  void ID##_internal_mavl_free_help(const ID##_mavl_ptr_t *const self,         \
                                    ID##_mavl_node_t *self_node) {             \
    if (*self_node == self->nil) {                                             \
      return;                                                                  \
    }                                                                          \
                                                                               \
    ID##_internal_mavl_free_help(self, &(*self_node)->left);                   \
    ID##_internal_mavl_free_help(self, &(*self_node)->right);                  \
                                                                               \
    if (self->frd != NULL) {                                                   \
      self->frd(&(*self_node)->data);                                          \
    }                                                                          \
                                                                               \
    free(*self_node);                                                          \
    *self_node = self->nil;                                                    \
  }                                                                            \
                                                                               \
  merr_t ID##_mavl_free(ID##_mavl_t *self) {                                   \
    if ((self != NULL) && (*self != NULL)) {                                   \
      ID##_internal_mavl_free_help(*self, &(*self)->root);                     \
      free((*self)->nil);                                                      \
      free(*self);                                                             \
      *self = NULL;                                                            \
                                                                               \
      return M_OK;                                                             \
    }                                                                          \
                                                                               \
    return M_FREE_NULL;                                                        \
  }                                                                            \
                                                                               \
  void ID##_internal_mavl_upheight(const ID##_mavl_ptr_t *const self,          \
                                   ID##_mavl_node_t const self_node) {         \
    if (self_node != self->nil) {                                              \
      self_node->height =                                                      \
          MAVL_MAX_VALUES(self_node->left->height, self_node->right->height) + \
          1;                                                                   \
    }                                                                          \
  }                                                                            \
                                                                               \
  int32_t ID##_internal_mavl_balance(                                          \
      const ID##_mavl_node_ptr_t *const self_node) {                           \
    return ((int32_t)self_node->left->height -                                 \
            (int32_t)self_node->right->height);                                \
  }                                                                            \
                                                                               \
  void ID##_internal_mavl_rotl(ID##_mavl_t const self,                         \
                               ID##_mavl_node_t const self_node) {             \
    if ((self == NULL) || (self_node == self->nil) ||                          \
        (self_node->right == self->nil)) {                                     \
      return;                                                                  \
    }                                                                          \
                                                                               \
    ID##_mavl_node_t temp = self_node->right;                                  \
    self_node->right = temp->left;                                             \
                                                                               \
    if (temp->left != self->nil) {                                             \
      temp->left->parent = self_node;                                          \
    }                                                                          \
                                                                               \
    temp->left = self_node;                                                    \
    temp->parent = self_node->parent;                                          \
    self_node->parent = temp;                                                  \
                                                                               \
    if (temp->parent != self->nil) {                                           \
      if (self->cmp(&temp->data, &temp->parent->data) >= 1) {                  \
        temp->parent->right = temp;                                            \
      } else {                                                                 \
        temp->parent->left = temp;                                             \
      }                                                                        \
    } else {                                                                   \
      self->root = temp;                                                       \
    }                                                                          \
                                                                               \
    ID##_internal_mavl_upheight(self, self_node);                              \
    ID##_internal_mavl_upheight(self, temp);                                   \
  }                                                                            \
                                                                               \
  void ID##_internal_mavl_rotr(ID##_mavl_t const self,                         \
                               ID##_mavl_node_t const self_node) {             \
    if ((self == NULL) || (self_node == self->nil) ||                          \
        (self_node->left == self->nil)) {                                      \
      return;                                                                  \
    }                                                                          \
                                                                               \
    ID##_mavl_node_t temp = self_node->left;                                   \
    self_node->left = temp->right;                                             \
                                                                               \
    if (temp->right != self->nil) {                                            \
      temp->right->parent = self_node;                                         \
    }                                                                          \
                                                                               \
    temp->right = self_node;                                                   \
    temp->parent = self_node->parent;                                          \
    self_node->parent = temp;                                                  \
                                                                               \
    if (temp->parent != self->nil) {                                           \
      if (self->cmp(&temp->data, &temp->parent->data) >= 1) {                  \
        temp->parent->right = temp;                                            \
      } else {                                                                 \
        temp->parent->left = temp;                                             \
      }                                                                        \
    } else {                                                                   \
      self->root = temp;                                                       \
    }                                                                          \
                                                                               \
    ID##_internal_mavl_upheight(self, self_node);                              \
    ID##_internal_mavl_upheight(self, temp);                                   \
  }

/**
 * @brief Checks whether a binary
 * searc tree object is empty or not.
 */
#define MAVL_EMPTY(ID, T)                                                      \
  mbool_t ID##_mavl_empty(const ID##_mavl_ptr_t *const self) {                 \
    if ((self == NULL) || (self->root == self->nil) || (self->size == 0)) {    \
      return mtrue;                                                            \
    }                                                                          \
                                                                               \
    return mfalse;                                                             \
  }

/**
 * @brief Fetches the size of the binary search tree of unique nodes,
 * which means that the count of a single node is not taken in account.
 */
#define MAVL_SIZE(ID, T)                                                       \
  size_t ID##_mavl_size(const ID##_mavl_ptr_t *const self) {                   \
    if (self == NULL) {                                                        \
      return SIZE_MAX;                                                         \
    }                                                                          \
                                                                               \
    return self->size;                                                         \
  }

/**
 * @brief Gets the data under the root node of the binary search tree.
 */
#define MAVL_ROOT(ID, T)                                                       \
  merr_t ID##_mavl_root(const ID##_mavl_ptr_t *const self, T *const acc) {     \
    if ((self == NULL) || (acc == NULL)) {                                     \
      return M_NULL_INPUT;                                                     \
    }                                                                          \
                                                                               \
    if (self->root == self->nil) {                                             \
      return M_EMPTY_STRUCTURE;                                                \
    }                                                                          \
                                                                               \
    *acc = self->root->data;                                                   \
                                                                               \
    return M_OK;                                                               \
  }

#define MAVL_FIND_NODE(ID, T)                                                  \
  ID##_mavl_node_t ID##_internal_mavl_find_node(                               \
      const ID##_mavl_ptr_t *const self, T *const data) {                      \
    if (self == NULL) {                                                        \
      return self->nil;                                                        \
    }                                                                          \
                                                                               \
    ID##_mavl_node_t iterator = self->root;                                    \
                                                                               \
    while (iterator != self->nil) {                                            \
      if (self->cmp(&iterator->data, data) <= -1) {                            \
        iterator = iterator->right;                                            \
      } else if (self->cmp(&iterator->data, data) >= 1) {                      \
        iterator = iterator->left;                                             \
      } else {                                                                 \
        return iterator;                                                       \
      }                                                                        \
    }                                                                          \
                                                                               \
    return self->nil;                                                          \
  }

/**
 * @brief Finds a data inside the binary search tree. The function works mostly
 * like a `contains` method, with extra accumulator field, in order to fetch the
 * data from the node, it is needed if you have pointers fields which can be
 * modified, thus the cmp function MUST not depend on the pointer data,
 * otherwise it may break the structure of the binary search tree.
 *
 */
#define MAVL_FIND(ID, T)                                                       \
  merr_t ID##_mavl_find(const ID##_mavl_ptr_t *const self, T data,             \
                        T *const acc) {                                        \
    if ((self == NULL)) {                                                      \
      return M_NULL_INPUT;                                                     \
    }                                                                          \
                                                                               \
    ID##_mavl_node_t self_node = ID##_internal_mavl_find_node(self, &data);    \
                                                                               \
    if (self_node == self->nil) {                                              \
      return M_NOT_FOUND;                                                      \
    }                                                                          \
                                                                               \
    if (acc != NULL) {                                                         \
      *acc = self_node->data;                                                  \
    }                                                                          \
                                                                               \
    return M_OK;                                                               \
  }

/**
 * @brief Finds the minimum data of the binary search tree, works just like
 * mavl_find, with fetching the minimum node.
 */
#define MAVL_MIN(ID, T)                                                        \
  ID##_mavl_node_t ID##_internal_mavl_min_node(                                \
      const ID##_mavl_ptr_t *const self, ID##_mavl_node_t self_node) {         \
    if (self_node != self->nil) {                                              \
      while (self_node->left != self->nil) {                                   \
        self_node = self_node->left;                                           \
      }                                                                        \
    }                                                                          \
                                                                               \
    return self_node;                                                          \
  }                                                                            \
                                                                               \
  merr_t ID##_mavl_min(const ID##_mavl_ptr_t *const self, T data,              \
                       T *const acc) {                                         \
    if ((self == NULL) || (acc == NULL)) {                                     \
      return M_NULL_INPUT;                                                     \
    }                                                                          \
                                                                               \
    *acc = ID##_internal_mavl_min_node(                                        \
               self, ID##_internal_mavl_find_node(self, &data))                \
               ->data;                                                         \
                                                                               \
    return M_OK;                                                               \
  }

/**
 * @brief Finds the maximumdata of the binary search tree, works just like
 * mavl_find, with fetching the maximum node.
 */
#define MAVL_MAX(ID, T)                                                        \
  ID##_mavl_node_t ID##_internal_mavl_max_node(                                \
      const ID##_mavl_ptr_t *const self, ID##_mavl_node_t self_node) {         \
    if (self_node != self->nil) {                                              \
      while (self_node->right != self->nil) {                                  \
        self_node = self_node->right;                                          \
      }                                                                        \
    }                                                                          \
                                                                               \
    return self_node;                                                          \
  }                                                                            \
                                                                               \
  merr_t ID##_mavl_max(const ID##_mavl_ptr_t *const self, T data,              \
                       T *const acc) {                                         \
    if ((self == NULL) || (acc == NULL)) {                                     \
      return M_NULL_INPUT;                                                     \
    }                                                                          \
                                                                               \
    *acc = ID##_internal_mavl_max_node(                                        \
               self, ID##_internal_mavl_find_node(self, &data))                \
               ->data;                                                         \
                                                                               \
    return M_OK;                                                               \
  }

/**
 * @brief Finds the predecessor data of the binary search tree, works just like
 * mavl_find, with fetching the predecessor node.
 */
#define MAVL_PRED(ID, T)                                                       \
  merr_t ID##_mavl_pred(const ID##_mavl_ptr_t *const self, T data,             \
                        T *const acc) {                                        \
    if (self == NULL) {                                                        \
      return M_NULL_INPUT;                                                     \
    }                                                                          \
                                                                               \
    ID##_mavl_node_t iterator = ID##_internal_mavl_find_node(self, &data);     \
                                                                               \
    if (iterator == self->nil) {                                               \
      return M_INVALID_INPUT;                                                  \
    }                                                                          \
                                                                               \
    if (acc != NULL) {                                                         \
      if (iterator->left != self->nil) {                                       \
        *acc = ID##_internal_mavl_max_node(self, iterator->left)->data;        \
      } else {                                                                 \
        ID##_mavl_node_t parent_iterator = iterator->parent;                   \
                                                                               \
        while ((parent_iterator != self->nil) &&                               \
               (parent_iterator->left == iterator)) {                          \
          iterator = parent_iterator;                                          \
          parent_iterator = parent_iterator->parent;                           \
        }                                                                      \
                                                                               \
        *acc = parent_iterator->data;                                          \
      }                                                                        \
    }                                                                          \
                                                                               \
    return M_OK;                                                               \
  }

/**
 * @brief Finds the successor data of the binary search tree, works just like
 * mavl_find, with fetching the successor node.
 */
#define MAVL_SUCC(ID, T)                                                       \
  merr_t ID##_mavl_succ(const ID##_mavl_ptr_t *const self, T data,             \
                        T *const acc) {                                        \
    if (self == NULL) {                                                        \
      return M_NULL_INPUT;                                                     \
    }                                                                          \
                                                                               \
    ID##_mavl_node_t iterator = ID##_internal_mavl_find_node(self, &data);     \
                                                                               \
    if (iterator == self->nil) {                                               \
      return M_INVALID_INPUT;                                                  \
    }                                                                          \
                                                                               \
    if (acc != NULL) {                                                         \
      if (iterator->right != self->nil) {                                      \
        *acc = ID##_internal_mavl_min_node(self, iterator->right)->data;       \
      } else {                                                                 \
        ID##_mavl_node_t parent_iterator = iterator->parent;                   \
                                                                               \
        while ((parent_iterator != self->nil) &&                               \
               (parent_iterator->right == iterator)) {                         \
          iterator = parent_iterator;                                          \
          parent_iterator = parent_iterator->parent;                           \
        }                                                                      \
                                                                               \
        *acc = parent_iterator->data;                                          \
      }                                                                        \
    }                                                                          \
                                                                               \
    return M_OK;                                                               \
  }

/**
 * @brief Finds the lowest common ancestor data of the binary search tree, works
 * just like mavl_find.
 */
#define MAVL_LCA(ID, T)                                                        \
  merr_t ID##_mavl_lca(const ID##_mavl_ptr_t *const self, T left, T right,     \
                       T *const acc) {                                         \
    if (self == NULL) {                                                        \
      return M_NULL_INPUT;                                                     \
    }                                                                          \
                                                                               \
    if ((ID##_internal_mavl_find_node(self, &left) == self->nil) ||            \
        (ID##_internal_mavl_find_node(self, &right) == self->nil)) {           \
      return M_INVALID_INPUT;                                                  \
    }                                                                          \
                                                                               \
    if (acc != NULL) {                                                         \
      ID##_mavl_node_t iterator = self->root;                                  \
                                                                               \
      while (iterator != self->nil) {                                          \
        if ((self->cmp(&iterator->data, &left) >= 1) &&                        \
            (self->cmp(&iterator->data, &right) >= 1)) {                       \
          iterator = iterator->left;                                           \
        } else if ((self->cmp(&iterator->data, &left) <= -1) &&                \
                   (self->cmp(&iterator->data, &right) <= -1)) {               \
          iterator = iterator->right;                                          \
        } else {                                                               \
          *acc = iterator->data;                                               \
                                                                               \
          return M_OK;                                                         \
        }                                                                      \
      }                                                                        \
                                                                               \
      return M_UNDEFINED_BEHAVIOUR;                                            \
    }                                                                          \
                                                                               \
    return M_OK;                                                               \
  }

/**
 * @brief Inserts a new data inside the binary search tree, it is important to
 * specify a valid cmp function in order to arange data inside the binary search
 * tree.
 */
#define MAVL_PUSH(ID, T)                                                       \
  void ID##_internal_mavl_push_fix(ID##_mavl_t const self,                     \
                                   ID##_mavl_node_t self_node) {               \
    while (self_node != self->nil) {                                           \
      ID##_internal_mavl_upheight(self, self_node);                            \
                                                                               \
      int32_t bf = ID##_internal_mavl_balance(self_node);                      \
      int32_t lbf = ID##_internal_mavl_balance(self_node->left);               \
      int32_t rbf = ID##_internal_mavl_balance(self_node->right);              \
                                                                               \
      if ((bf == 2) && (lbf == 1)) {                                           \
        ID##_internal_mavl_rotr(self, self_node);                              \
      }                                                                        \
                                                                               \
      if ((bf == -2) && (rbf == -1)) {                                         \
        ID##_internal_mavl_rotl(self, self_node);                              \
      }                                                                        \
                                                                               \
      if ((bf == 2) && (lbf == -1)) {                                          \
        ID##_internal_mavl_rotl(self, self_node->left);                        \
        ID##_internal_mavl_rotr(self, self_node);                              \
      }                                                                        \
                                                                               \
      if ((bf == -2) && (rbf == 1)) {                                          \
        ID##_internal_mavl_rotr(self, self_node->right);                       \
        ID##_internal_mavl_rotl(self, self_node);                              \
      }                                                                        \
                                                                               \
      self_node = self_node->parent;                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  merr_t ID##_mavl_push(ID##_mavl_t const self, T data) {                      \
    if (self == NULL) {                                                        \
      return M_NULL_INPUT;                                                     \
    }                                                                          \
                                                                               \
    ID##_mavl_node_t iterator = self->root;                                    \
    ID##_mavl_node_t parent_iterator = self->nil;                              \
                                                                               \
    while (iterator != self->nil) {                                            \
      parent_iterator = iterator;                                              \
                                                                               \
      if (self->cmp(&iterator->data, &data) >= 1) {                            \
        iterator = iterator->left;                                             \
      } else if (self->cmp(&iterator->data, &data) <= -1) {                    \
        iterator = iterator->right;                                            \
      } else {                                                                 \
        ++(iterator->count);                                                   \
        return M_OK;                                                           \
      }                                                                        \
    }                                                                          \
                                                                               \
    ID##_mavl_node_t self_node = ID##_internal_mavl_node(self, &data);         \
                                                                               \
    if (self_node == self->nil) {                                              \
      return M_MALLOC_FAILED;                                                  \
    }                                                                          \
                                                                               \
    if (parent_iterator != self->nil) {                                        \
      self_node->parent = parent_iterator;                                     \
                                                                               \
      if (self->cmp(&parent_iterator->data, &self_node->data) >= 1) {          \
        parent_iterator->left = self_node;                                     \
      } else {                                                                 \
        parent_iterator->right = self_node;                                    \
      }                                                                        \
                                                                               \
      ID##_internal_mavl_push_fix(self, parent_iterator);                      \
    } else {                                                                   \
      self->root = self_node;                                                  \
    }                                                                          \
                                                                               \
    ++(self->size);                                                            \
                                                                               \
    return M_OK;                                                               \
  }

/**
 * @brief Removes a single data node from the binary search tree and frees its
 * content using the frd function specified at the creation of the object.
 *
 */
#define MAVL_POP(ID, T)                                                        \
  void ID##_internal_mavl_swap(ID##_mavl_t const self,                         \
                               ID##_mavl_node_t const dest,                    \
                               ID##_mavl_node_t const src) {                   \
    if ((dest == self->nil) || (src == self->nil)) {                           \
      return;                                                                  \
    }                                                                          \
                                                                               \
    ID##_mavl_node_t temp = dest->right;                                       \
    dest->right = src->right;                                                  \
                                                                               \
    if (dest->right != self->nil) {                                            \
      dest->right->parent = dest;                                              \
    }                                                                          \
                                                                               \
    src->right = temp;                                                         \
                                                                               \
    if (src->right != self->nil) {                                             \
      src->right->parent = src;                                                \
    }                                                                          \
                                                                               \
    temp = dest->left;                                                         \
    dest->left = src->left;                                                    \
                                                                               \
    if (dest->left != self->nil) {                                             \
      dest->left->parent = dest;                                               \
    }                                                                          \
                                                                               \
    src->left = temp;                                                          \
                                                                               \
    if (src->left != self->nil) {                                              \
      src->left->parent = src;                                                 \
    }                                                                          \
                                                                               \
    temp = dest->parent;                                                       \
    dest->parent = src->parent;                                                \
                                                                               \
    if (dest->parent != self->nil) {                                           \
      if (dest->parent->left == src) {                                         \
        dest->parent->left = dest;                                             \
      } else {                                                                 \
        dest->parent->right = dest;                                            \
      }                                                                        \
    } else {                                                                   \
      self->root = dest;                                                       \
    }                                                                          \
                                                                               \
    src->parent = temp;                                                        \
                                                                               \
    if (src->parent != self->nil) {                                            \
      if (src->parent->left == dest) {                                         \
        src->parent->left = src;                                               \
      } else {                                                                 \
        src->parent->right = src;                                              \
      }                                                                        \
    } else {                                                                   \
      self->root = src;                                                        \
    }                                                                          \
  }                                                                            \
                                                                               \
  void ID##_internal_mavl_pop_fix(ID##_mavl_t const self,                      \
                                  ID##_mavl_node_t self_node) {                \
    while (self_node != self->nil) {                                           \
      ID##_internal_mavl_upheight(self, self_node);                            \
                                                                               \
      int32_t bf = ID##_internal_mavl_balance(self_node);                      \
      int32_t lbf = ID##_internal_mavl_balance(self_node->left);               \
      int32_t rbf = ID##_internal_mavl_balance(self_node->right);              \
                                                                               \
      if ((bf > 1) && (lbf >= 0)) {                                            \
        ID##_internal_mavl_rotr(self, self_node);                              \
      }                                                                        \
                                                                               \
      if ((bf < -1) && (rbf <= 0)) {                                           \
        ID##_internal_mavl_rotl(self, self_node);                              \
      }                                                                        \
                                                                               \
      if ((bf > 1) && (lbf < 0)) {                                             \
        ID##_internal_mavl_rotl(self, self_node->left);                        \
        ID##_internal_mavl_rotr(self, self_node);                              \
      }                                                                        \
                                                                               \
      if ((bf < -1) && (rbf > 0)) {                                            \
        ID##_internal_mavl_rotr(self, self_node->right);                       \
        ID##_internal_mavl_rotl(self, self_node);                              \
      }                                                                        \
                                                                               \
      self_node = self_node->parent;                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  merr_t ID##_mavl_pop(ID##_mavl_t const self, T data) {                       \
    if (self == NULL) {                                                        \
      return M_NULL_INPUT;                                                     \
    }                                                                          \
                                                                               \
    if (self->root == self->nil) {                                             \
      return M_POP_FROM_EMPTY;                                                 \
    }                                                                          \
                                                                               \
    ID##_mavl_node_t self_node = ID##_internal_mavl_find_node(self, &data);    \
                                                                               \
    if (self_node == self->nil) {                                              \
      return M_INVALID_INPUT;                                                  \
    }                                                                          \
                                                                               \
    if ((self_node->left != self->nil) && (self_node->right != self->nil)) {   \
      ID##_internal_mavl_swap(                                                 \
          self, self_node,                                                     \
          ID##_internal_mavl_min_node(self, self_node->right));                \
    }                                                                          \
                                                                               \
    if (self_node->left != self->nil) {                                        \
      self_node->left->parent = self_node->parent;                             \
                                                                               \
      if (self_node->parent != self->nil) {                                    \
        if (self_node->parent->right == self_node) {                           \
          self_node->parent->right = self_node->left;                          \
        } else {                                                               \
          self_node->parent->left = self_node->left;                           \
        }                                                                      \
      } else {                                                                 \
        self->root = self_node->left;                                          \
      }                                                                        \
    } else if (self_node->right != self->nil) {                                \
      self_node->right->parent = self_node->parent;                            \
                                                                               \
      if (self_node->parent != self->nil) {                                    \
        if (self_node->parent->right == self_node) {                           \
          self_node->parent->right = self_node->right;                         \
        } else {                                                               \
          self_node->parent->left = self_node->right;                          \
        }                                                                      \
      } else {                                                                 \
        self->root = self_node->right;                                         \
      }                                                                        \
    } else {                                                                   \
      if (self_node->parent != self->nil) {                                    \
        if (self_node->parent->right == self_node) {                           \
          self_node->parent->right = self->nil;                                \
        } else {                                                               \
          self_node->parent->left = self->nil;                                 \
        }                                                                      \
      } else {                                                                 \
        self->root = self->nil;                                                \
      }                                                                        \
    }                                                                          \
                                                                               \
    ID##_mavl_node_t parent_self_node = self_node->parent;                     \
                                                                               \
    if (self->frd != NULL) {                                                   \
      self->frd(&self_node->data);                                             \
    }                                                                          \
                                                                               \
    free(self_node);                                                           \
                                                                               \
    --(self->size);                                                            \
                                                                               \
    if (parent_self_node != self->nil) {                                       \
      ID##_internal_mavl_pop_fix(self, parent_self_node);                      \
    }                                                                          \
                                                                               \
    return M_OK;                                                               \
  }

/**
 * @brief Traversing the tree inorder method.
 */
#define MAVL_TRAVERSE_INORDER(ID, T)                                           \
  ACTION_FUNC(ID, T)                                                           \
                                                                               \
  void ID##_internal_mavl_traverse_inorder_help(                               \
      const ID##_mavl_ptr_t *const self,                                       \
      const ID##_mavl_node_ptr_t *const self_node, ID##_action_func action) {  \
    if (self_node == self->nil) {                                              \
      return;                                                                  \
    }                                                                          \
                                                                               \
    ID##_internal_mavl_traverse_inorder_help(self, self_node->left, action);   \
    action(&self_node->data);                                                  \
    ID##_internal_mavl_traverse_inorder_help(self, self_node->right, action);  \
  }                                                                            \
                                                                               \
  merr_t ID##_mavl_traverse_inorder(const ID##_mavl_ptr_t *const self,         \
                                    ID##_action_func action) {                 \
    if (self == NULL) {                                                        \
      return M_NULL_INPUT;                                                     \
    }                                                                          \
                                                                               \
    if (action == NULL) {                                                      \
      return M_NULL_ACTION;                                                    \
    }                                                                          \
                                                                               \
    if (self->root == self->nil) {                                             \
      printf("(Nil)\n");                                                       \
    } else {                                                                   \
      printf("[");                                                             \
      ID##_internal_mavl_traverse_inorder_help(self, self->root, action);      \
      printf(" ]");                                                            \
    }                                                                          \
                                                                               \
    return M_OK;                                                               \
  }

/**
 * @brief Traversing the tree preorder method.
 */
#define MAVL_TRAVERSE_PREORDER(ID, T)                                          \
  ACTION_FUNC(ID, T)                                                           \
                                                                               \
  void ID##_internal_mavl_traverse_preorder_help(                              \
      const ID##_mavl_ptr_t *const self,                                       \
      const ID##_mavl_node_ptr_t *const self_node, ID##_action_func action) {  \
    if (self_node == self->nil) {                                              \
      return;                                                                  \
    }                                                                          \
                                                                               \
    action(&self_node->data);                                                  \
    ID##_internal_mavl_traverse_preorder_help(self, self_node->left, action);  \
    ID##_internal_mavl_traverse_preorder_help(self, self_node->right, action); \
  }                                                                            \
                                                                               \
  merr_t ID##_mavl_traverse_preorder(const ID##_mavl_ptr_t *const self,        \
                                     ID##_action_func action) {                \
    if (self == NULL) {                                                        \
      return M_NULL_INPUT;                                                     \
    }                                                                          \
                                                                               \
    if (action == NULL) {                                                      \
      return M_NULL_ACTION;                                                    \
    }                                                                          \
                                                                               \
    if (self->root == self->nil) {                                             \
      printf("(Nil)\n");                                                       \
    } else {                                                                   \
      printf("[");                                                             \
      ID##_internal_mavl_traverse_preorder_help(self, self->root, action);     \
      printf(" ]");                                                            \
    }                                                                          \
                                                                               \
    return M_OK;                                                               \
  }

/**
 * @brief Traversing the tree postorder method.
 */
#define MAVL_TRAVERSE_POSTORDER(ID, T)                                         \
  ACTION_FUNC(ID, T)                                                           \
                                                                               \
  void ID##_internal_mavl_traverse_postorder_help(                             \
      const ID##_mavl_ptr_t *const self,                                       \
      const ID##_mavl_node_ptr_t *const self_node, ID##_action_func action) {  \
    if (self_node == self->nil) {                                              \
      return;                                                                  \
    }                                                                          \
                                                                               \
    action(&self_node->data);                                                  \
    ID##_internal_mavl_traverse_postorder_help(self, self_node->left, action); \
    ID##_internal_mavl_traverse_postorder_help(self, self_node->right,         \
                                               action);                        \
  }                                                                            \
                                                                               \
  merr_t ID##_mavl_traverse_postorder(const ID##_mavl_ptr_t *const self,       \
                                      ID##_action_func action) {               \
    if (self == NULL) {                                                        \
      return M_NULL_INPUT;                                                     \
    }                                                                          \
                                                                               \
    if (action == NULL) {                                                      \
      return M_NULL_ACTION;                                                    \
    }                                                                          \
                                                                               \
    if (self->root == self->nil) {                                             \
      printf("(Nil)\n");                                                       \
    } else {                                                                   \
      printf("[");                                                             \
      ID##_internal_mavl_traverse_postorder_help(self, self->root, action);    \
      printf(" ]");                                                            \
    }                                                                          \
                                                                               \
    return M_OK;                                                               \
  }

/**
 * @brief Adds the all API for the `mavl_t` structure (binary search tree). You
 * will not be always need to use all the API, in this case you must be sure
 * that you call `MAVL` for definition, any other macro definitions are to bring
 * new functionalities. The code length may be reduced a lot if you do not call
 * `MAVL_ALL`, the code duplicated when calling `MAVL_ALL` for different ids or
 * different types, it is encoureged to not to use `MAVL_ALL` or not to declare
 * different ids for the same type. The ID protocol is used when different files
 * want to have the same two typed structures in order to avoid name collisions.
 */
#define MAVL_ALL(ID, T)                                                        \
  MAVL(ID, T)                                                                  \
  MAVL_EMPTY(ID, T)                                                            \
  MAVL_SIZE(ID, T)                                                             \
  MAVL_ROOT(ID, T)                                                             \
  MAVL_FIND_NODE(ID, T)                                                        \
  MAVL_FIND(ID, T)                                                             \
  MAVL_MIN(ID, T)                                                              \
  MAVL_MAX(ID, T)                                                              \
  MAVL_PRED(ID, T)                                                             \
  MAVL_SUCC(ID, T)                                                             \
  MAVL_LCA(ID, T)                                                              \
  MAVL_PUSH(ID, T)                                                             \
  MAVL_POP(ID, T)                                                              \
  MAVL_TRAVERSE_INORDER(ID, T)                                                 \
  MAVL_TRAVERSE_PREORDER(ID, T)                                                \
  MAVL_TRAVERSE_POSTORDER(ID, T)

#endif /* MACROS_GENERIC_AVL_BINARY_SEARCH_TREE_UTILS_H_ */

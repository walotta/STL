/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    > class map {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;
    private:
        static Compare isLess;
        int cnt=0;
        inline bool equal(const Key& a,const Key& b)const{return isLess(a,b)&&isLess(b,a);}
        inline bool small_key(const Key& a,const Key& b)const{return isLess(a,b);}
        class node
        {
        public:
            pair<const Key, T>* value=nullptr;
            node* father=nullptr;
            node* left_son=nullptr;
            node* right_son=nullptr;
            node()=default;
            ~node(){if(value!=nullptr)delete value;}
            inline bool isLeft()const{return father->left_son==this;}
            inline bool isRight()const{return father->right_son==this;}
            inline bool isRoot()const{return father->value==nullptr&&father->father==this;}
            inline void turn_right()
            {
                father->left_son=right_son;
                right_son->father=father;
                right_son=father;
                right_son->father=this;
                father=father->father;
                if(right_son->isRight())right_son->father->right_son=this;
                else right_son->father->left_son=this;
            }
            inline void turn_left()
            {
                father->right_son=left_son;
                left_son->father=father;
                left_son=father;
                left_son->father=this;
                father=father->father;
                if(left_son->isRight())left_son->father->right_son=this;
                else left_son->father->left_son=this;
            }
            inline void change_with_father()
            {
                if(isRight())turn_right();
                else turn_left();
            }
            inline void splay()
            {
                while(!isRoot())change_with_father();
            }
            inline node* min_son()const
            {
                node* it=const_cast<node*>(this);
                while(it->left_son!=nullptr)it=it->left_son;
                return it;
            }
            inline node* max_son()const
            {
                node* it=const_cast<node*>(this);
                while(it->right_son!=nullptr)it=it->left_son;
                return it;
            }
            inline node* next()const
            {
                if(right_son!=nullptr)return right_son->min_son();
                else
                {
                    node* it=const_cast<node*>(this);
                    while(it->isRight())it=it->father;
                    return it->father;
                }
            }
            inline node* front()const
            {
                if(left_son!=nullptr)return left_son->max_son();
                else
                {
                    node* it=const_cast<node*>(this);
                    while(it->isLeft())it=it->father;
                    return it->father;
                }
            }
        };
        node* root;
        void DFS_copy_tree(node* my_node,node* target_node)
        {
            if(target_node->value!=nullptr)my_node->value=new value_type(*(target_node->value));
            if(target_node->left_son!=nullptr)
            {
                my_node->left_son=new node;
                my_node->left_son->father=my_node;
                DFS_copy_tree(my_node->left_son,target_node->left_son);
            }
            if(target_node->right_son!=nullptr)
            {
                my_node->right_son=new node;
                my_node->right_son->father=my_node;
                DFS_copy_tree(my_node->right_son,target_node->right_son);
            }
        }
        void DFS_delete(node* now)
        {
            if(now->left_son!=nullptr)DFS_delete(now->left_son);
            if(now->right_son!=nullptr)DFS_delete(now->right_son);
            if(now->isRight())now->father->right_son=nullptr;
            else now->father->left_son=nullptr;
            delete now;
        }
        node* DFS_find(const Key &key,node* now)const
        {
            if(now==nullptr)return nullptr;
            if(equal(key,now->value->first))return now;
            else if(small_key(key,now->value->first))return DFS_find(key,now->left_son);
            else return DFS_find(key,now->right_son);
        }
        node* DFS_insert(const Key& key,node* now)
        {
            if(small_key(key,now->value->first))
            {
                if(now->left_son==nullptr)
                {
                    now->left_son=new node;
                    now->left_son->father=now;
                    now->left_son->value=new value_type(key,T());
                    if(root->left_son==now)root->left_son=now->left_son;
                    return now->left_son;
                }else return DFS_insert(key,now->left_son);
            }else
            {
                if(now->right_son==nullptr)
                {
                    now->right_son=new node;
                    now->right_son->father=now;
                    now->right_son->value=new value_type(key,T());
                    if(root->right_son==now)root->right_son=now->right_son;
                    return now->right_son;
                }else return DFS_insert(key,now->right_son);
            }
        }
        inline void copy_from_other_tree(const map &other)
        {
            DFS_copy_tree(root,other.root);
            cnt=other.cnt;
        }
        inline void reset_tree()
        {
            DFS_delete(root);
            root=new node;
            cnt=0;
        }
        inline void merge_tree(node* left_root,node* right_root)
        {
            left_root->father->splay();
            left_root->father->right_son=right_root->father;
            right_root->father->father=left_root->father;
        }
        inline int tree_count(const Key& key)const
        {
            node* ans=DFS_find(key,root);
            if(ans==nullptr)return 0;
            else return 1;
        }
        inline node* tree_find(const Key &key)const
        {
            node* ans=DFS_find(key,root);
            if(ans==nullptr)return nullptr;
            ans->splay();
            root->father=ans;
            return ans;
        }
        inline node* tree_insert(const Key& key)
        {
            //todo
            cnt++;
            if(root->father==root)
            {
                //树为空
                root->father=new node;
                root->left_son=root->father;
                root->right_son=root->father;
                root->father->father=root;
                root->father->value=new value_type(key,T());
                return root->father;
            }else
            {
                node* ans=DFS_insert(key,root->father);
                ans->splay();
                if(small_key(ans->value->first,root->left_son->value->first))root->left_son=ans;
                if(small_key(root->right_son->value->first,ans->value->first))root->right_son=ans;
                return ans;
            }
        }
        inline void remove_node(node* to_remove)
        {
            if(to_remove==nullptr)throw index_out_of_bound();
            cnt--;
            to_remove->splay();
            if(to_remove->left_son==nullptr&&to_remove->right_son==nullptr)
            {
                delete to_remove;
                root->father=root;
                root->left_son=nullptr;
                root->right_son=nullptr;
                return;
            }
            if(equal(to_remove->value->first,root->left_son->value->first))root->left_son=to_remove->next();
            if(equal(to_remove->value->first,root->right_son->value->first))root->right_son=to_remove->front();
            if(to_remove->left_son==nullptr)
            {
                root->father=to_remove->right_son;
                to_remove->right_son->father=root;
                delete to_remove;
            }else if(to_remove->right_son==nullptr)
            {
                root->father=to_remove->left_son;
                to_remove->left_son->father=root;
                delete to_remove;
            }else
            {
                root->father=to_remove;
                node* left_root=new node;
                node* right_root=new node;
                left_root->father=to_remove->left_son;
                to_remove->left_son->father=left_root;
                right_root->father=to_remove->right_son;
                to_remove->right_son->father=right_root;
                merge_tree(left_root,right_root);
                root->father=left_root->father;
                root->father->father=root;
            }
        }
        inline void tree_remove(const Key& key)
        {
            node* to_remove=DFS_find(key,root->father);
            remove_node(to_remove);
        }
    public:
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;
        class iterator {
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
             node* target=nullptr;
             map* belongs=nullptr;
             template<class X,class Y,class Z>
             friend class map;
        public:
            iterator()=default;
            iterator(node* _tar,map* _bel):target(_tar),belongs(_bel){}
            iterator(const iterator &other):target(other.target),belongs(other.belongs) {}
            /**
             * TODO iter++
             */
            iterator operator++(int) {
                auto tem=*this;
                target=target->next();
                return tem;
            }
            /**
             * TODO ++iter
             */
            iterator & operator++() {
                target=target->next();
                return *this;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int) {
                auto tem=*this;
                target=target->front();
                return tem;
            }
            /**
             * TODO --iter
             */
            iterator & operator--() {
                target=target->front();
                return *this;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type & operator*() const {
                return *(target->value);
            }
            bool operator==(const iterator &rhs) const {
                return (belongs==rhs.belongs&&target==rhs.target);
            }
            bool operator==(const const_iterator &rhs) const {
                return (belongs==rhs.belongs&&target==rhs.target);
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return (belongs!=rhs.belongs||target!=rhs.target);
            }
            bool operator!=(const const_iterator &rhs) const {
                return (belongs!=rhs.belongs||target!=rhs.target);
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type* operator->() const noexcept {
                return target->value;
            }
        };
        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // data members.
            const node* target=nullptr;
            const map* belongs=nullptr;
            template<class X,class Y,class Z>
            friend class map;
        public:
            const_iterator()=default;
            const_iterator(const node* _tar,const map* _bel):target(_tar),belongs(_bel){}
            const_iterator(const const_iterator &other):target(other.target),belongs(other.belongs){}
            const_iterator(const iterator &other):target(other.target),belongs(other.belongs){}
            const_iterator operator++(int) {
                auto tem=*this;
                target=target->next();
                return tem;
            }
            const_iterator & operator++() {
                target=target->next();
                return *this;
            }
            const_iterator operator--(int) {
                auto tem=*this;
                target=target->front();
                return tem;
            }
            const_iterator & operator--() {
                target=target->front();
                return *this;
            }
            const value_type & operator*() const {
                return *(target->value);
            }
            bool operator==(const iterator &rhs) const {
                return (belongs==rhs.belongs&&target==rhs.target);
            }
            bool operator==(const const_iterator &rhs) const {
                return (belongs==rhs.belongs&&target==rhs.target);
            }
            bool operator!=(const iterator &rhs) const {
                return (belongs!=rhs.belongs||target!=rhs.target);
            }
            bool operator!=(const const_iterator &rhs) const {
                return (belongs!=rhs.belongs||target!=rhs.target);
            }
            const value_type* operator->() const noexcept {
                return target->value;
            }
        };
        /**
         * TODO two constructors
         */
        map() {
            root=new node;
        }
        map(const map &other) {
            root=new node;
            copy_from_other_tree(other);
        }
        /**
         * TODO assignment operator
         */
        map & operator=(const map &other) {
            if(this==&other)return *this;
            reset_tree();
            copy_from_other_tree(other);
            return *this;
        }
        /**
         * TODO Destructors
         */
        ~map() {
            reset_tree();
            delete root;
        }
        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T & at(const Key &key) {
            node* ans=tree_find(key);
            if(ans==nullptr)throw index_out_of_bound();
            return ans->value->second;
        }
        const T & at(const Key &key) const {
            node* ans=tree_find(key);
            if(ans==nullptr)throw index_out_of_bound();
            return ans->value->second;
        }
        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T & operator[](const Key &key) {
            node* ans=tree_find(key);
            if(ans==nullptr)return tree_insert(key)->value->second;
            else return ans->value->second;
        }
        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T & operator[](const Key &key) const {
            node* ans=tree_find(key);
            if(ans==nullptr)throw index_out_of_bound();
            else return ans->value->second;
        }
        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            return iterator(root->left_son,this);
        }
        const_iterator cbegin() const {
            return const_iterator(root->left_son,this);
        }
        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(root,this);
        }
        const_iterator cend() const {
            return const_iterator(root,this);
        }
        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            return root->father==root;
        }
        /**
         * returns the number of elements.
         */
        size_t size() const {return cnt;}
        /**
         * clears the contents
         */
        void clear() {
            reset_tree();
        }
        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            node* exist=tree_find(value.first);
            if(exist==nullptr)
            {
                node* n=tree_insert(value.first);
                n->value->second=value.second;
                return pair<iterator,bool>(iterator(n,this),true);
            }else return pair<iterator,bool>(iterator(exist,this),true);
        }
        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if(pos==end())throw invalid_iterator();
            remove_node(pos.target);
        }
        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            return tree_count(key);
        }
        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key){
            node* ans=tree_find(key);
            if(ans==nullptr)return end();
            else return iterator(ans,this);
        }
        const_iterator find(const Key &key) const {
            node* ans=tree_find(key);
            if(ans==nullptr)return cend();
            else return const_iterator(ans,this);
        }
    };

}

#endif
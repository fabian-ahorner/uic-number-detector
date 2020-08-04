//
// Created by fab on 11.01.17.
//

#ifndef UIC_ITEMSELECTOR_H
#define UIC_ITEMSELECTOR_H

#include <vector>

using namespace std;

template<class Item>
class ItemSelector {
private:
    int currentIndex = 0;
    vector<Item> items;
protected:
    virtual void onSelect(int index, Item item);

public:
    inline ItemSelector(vector<Item> items): items(items) {

    }

    void next();

    void previous();

    Item getCurrent();

    vector<Item> getItems();

    int size();

    int getCurrentIndex();

    void setIndex(int index);

    void setItems(vector<Item> &items);
};

template<class Item>
void ItemSelector<Item>::next() {
    currentIndex = (currentIndex + 1) % (int) items.size();
    onSelect(currentIndex, getCurrent());
}

template<class Item>
void ItemSelector<Item>::previous() {
    currentIndex = (currentIndex + (int) items.size() - 1) % (int) items.size();
    onSelect(currentIndex, getCurrent());
}

template<class Item>
Item ItemSelector<Item>::getCurrent() {
    return items[currentIndex];
}

template<class Item>
vector<Item> ItemSelector<Item>::getItems() {
    return items;
}

template<class Item>
int ItemSelector<Item>::getCurrentIndex() {
    return currentIndex;
}

template<class Item>
int ItemSelector<Item>::size() {
    return (int) items.size();
}

template<class Item>
void ItemSelector<Item>::onSelect(int index, Item item) {
}

template<class Item>
void ItemSelector<Item>::setIndex(int index) {
//    assert(shift > 0 && shift < items.size());
    currentIndex = index;
    onSelect(currentIndex, getCurrent());
}

template<class Item>
void ItemSelector<Item>::setItems(vector<Item> &items) {
    this->items = items;
}

#endif //UIC_ITEMSELECTOR_H

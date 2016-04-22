/*
	data_node.cc
*/

#include "nodes/data_node.h" 

DataNode::DataNode(int dataLength,
	PID sidePter,
	int lowKey,
	int highKey) : Node(DATA) {

    data_ = new Pair<int, byte*>[dataLength];
	dataLength_ = dataLength;
	sidePter_ = sidePter;
	lowKey_ = lowKey;
	highKey_ = highKey;
}

DataNode::DataNode() : Node(DATA) {
	data_ = new Pair<int, byte*>[ARRAY_RECORDS_LENGTH];
	for(int i = 0; i < ARRAY_RECORDS_LENGTH; i++)
		data_[i].value = new byte[LENGTH_RECORDS];
}

void DataNode::setVariables(Pair<int, byte*>* data,
	int dataLength,
	PID sidePter,
	int lowKey,
	int highKey) {

	data_ = data;
	dataLength_ = dataLength;
	sidePter_ = sidePter;
	lowKey_ = lowKey;
	highKey_ = highKey;
}

int DataNode::pointToRecord(int key, byte ** record) {
	// binary search on the data_ array.
	if (key > highKey_) {
		*record = nullptr;
		return OVER_HIGH;
	}

	int left = 0, right = dataLength_, middle = 0, midVal = 0;
	while(left < right) {
		middle = left + (right-left)/2;
		midVal = data_[middle].key;
		
		if(midVal > key) {
			right = middle - 1;
		} else if (midVal < key) {
			left = middle + 1;
		} else {
			*record = data_[middle].value;
			return 0;
		}
	}
	
	*record = nullptr;
	return NOT_FOUND;
}

bool DataNode::doSplit() {
	return (dataLength_ > MAX_RECORDS);
}

int DataNode::getSplittingKey() {
	return data_[dataLength_ / 2].key;
}

int DataNode::getHighKey() {
	return highKey_;
}

int DataNode::getLowKey() {
	return lowKey_;
}

PID DataNode::getSidePtr() {
    return sidePter_;
};

int DataNode::getDataLength() {
	return dataLength_;
};

int DataNode::getDataKey(int i) {
    return data_[i].key; 
};

byte* DataNode::getDataVal(int i) {
	return data_[i].value;
};

void DataNode::setSidePter(PID sidePtr) {
    sidePter_ = sidePtr;    
};

void DataNode::setLowKey(int lowKey) {
	lowKey_ = lowKey;
};

void DataNode::setHighKey(int highKey) {
	highKey_ = highKey;
};

void DataNode::insertChainData(int key, byte *ptr) {
	// linear search to make sure it's not in the damn thing
    for (int i = 0; i < dataLength_; i++) {
        if (key == data_[i].key) {
            return;
        }
    }

    data_[dataLength_].key = key;
    data_[dataLength_].value = ptr;
    dataLength_++;
};

void DataNode::insertBaseData(int key, byte *val) {
    // linear search to make sure it's not in the damn thing
    data_[dataLength_].key = key;
    data_[dataLength_].value = val;
    dataLength_++;
};

bool DataNode::findSub(int key, int bound) {
    // binary search on the data_ array.
    int left = 0, right = bound, middle = 0, midVal = 0;
    while(left <= right) {
            middle = left + (right-left)/2;
            midVal = data_[middle].key;

            if(midVal > key) {
                    right = middle - 1;
            } else if (midVal < key) {
                    left = middle + 1;
            } else {
                    break;
            }
    }

    // if nothing found, return null. Else, return what is found.
    return (left < right);
}

void DataNode::merge(int low, int mid, int high, int dataLength) {
	int h,i,j,b[dataLength],k;
	h=low;
	i=low;
	j=mid+1;

	while((h<=mid)&&(j<=high)) {
		if(data_[h].key <= data_[j].key) {
			b[i]=data_[h].key;
			h++;
		} else {
			b[i]=data_[j].key;
			j++;
		}
		
		i++;
	}

	if(h>mid) {
		for(k=j;k<=high;k++) {
			b[i]=data_[k].key;
			i++;
		}
	} else {
		for(k=h;k<=mid;k++) {
			b[i]= data_[k].key;
			i++;
		}
	}

	for(k=low;k<=high;k++)  data_[k].key=b[k];
}

void DataNode::merge_sort(int low,int high, int dataLength) {
	int mid;
	if(low<high) {
	  mid = low + (high-low)/2; //This avoids overflow when low, high are too large
	  merge_sort(low,mid, dataLength);
	  merge_sort(mid+1,high, dataLength);
	  merge(low,mid,high, dataLength);
	}
}

void DataNode::mergesort() {
	merge_sort(0, dataLength_-1, dataLength_);
    // binary search on the data_ array.
//                        int key = 2;
//                        key++;
}

DataNode::~DataNode() {}
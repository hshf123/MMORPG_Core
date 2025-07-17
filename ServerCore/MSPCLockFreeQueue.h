#pragma once

template<class T>
class MSPCLockFreeQueue
{
private:
	struct Node
	{
		Node() : next(nullptr) {}
		Node(T&& value) : data(std::move(value)), next(nullptr) {}
		Node(const T& value) : data(value), next(nullptr) {}

		T data;
		std::atomic<Node*> next;
	};

public:
	MSPCLockFreeQueue()
	{
		Node* dummy = xnew<Node>();
		_head.store(dummy);
		_tail.store(dummy);
	}

	void Push(T&& item)
	{
		Node* newNode = xnew<Node>(std::move(item));
		Push(newNode);
	}
	void Push(const T& item)
	{
		Node* newNode = xnew<Node>(item);
		Push(newNode);
	}

	T Pop()
	{
		Node* oldHead = _head.load();
		Node* next = oldHead->next.load();

		if (next == nullptr)
			return T();

		T result = next->data;
		_head.store(next);
		xdelete(oldHead);
		return result;
	}
	void PopAll(std::vector<T>& items)
	{
		while (T dummy = Pop())
			items.push_back(dummy);
	}
	void Clear()
	{
		while (Pop());
	}

private:
	void Push(Node* newNode)
	{
		Node* oldTail = nullptr;

		while (true)
		{
			oldTail = _tail.load();
			Node* next = oldTail->next.load();
			if (next == nullptr)
			{
				if (oldTail->next.compare_exchange_strong(next, newNode))
					break;
			}
			else
			{
				_tail.compare_exchange_strong(oldTail, next);
			}
		}

		_tail.compare_exchange_strong(oldTail, newNode);
	}

	std::atomic<Node*> _head;
	std::atomic<Node*> _tail;
};
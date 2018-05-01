import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;


class PairLeaf {
	private int l;
	private int r;
	public PairLeaf(int l, int r){
		this.l = l;
		this.r = r;
	}
	
	public int getL(){ return l;}
	public int getR(){ return r;}
	public void setL(int l){this.l = l;}
	public void setR(int r){this.r = r;}
}

class PairIndex {
	private int l;
	private Node r;
	public PairIndex(int l, Node r){
		this.l = l;
		this.r = r;
	}
	
	public int getL(){ return l;}
	public Node getR(){ return r;}
	public void setL(int l){this.l = l;}
	public void setR(Node r){this.r = r;}
}


abstract class Node
{
	public int maxsize;
	public Node r;
	public IndexNode parent;
	public boolean isLeaf;
	
	public Node(int degree)
	{
		maxsize = degree;
		r = null;
		parent = null;
	}
	public int gethalf()
	{
		if(maxsize % 2 == 0)
			return maxsize/2;
		else
			return (maxsize+1)/2;
	}
	public abstract void delete(int place, Bplustree tree);
	public abstract int searchForInsert(int key);
	public abstract int searchKey(int key);
	public abstract void printSearch(int key);
	public abstract Node searchLeaf(int key);
	public abstract void traverse();	
}

class LeafNode extends Node
{
	public List<PairLeaf> p;
	public LeafNode l;
	
	public LeafNode(int degree)
	{
		super(degree);
		l = null;
		p = new ArrayList<PairLeaf>();
		super.isLeaf = true;
	}
	public void traverse()
	{
		System.out.print("[");
		for(int i=0; i<p.size(); i++)
		{
			System.out.print("(" + p.get(i).getL()+", "+p.get(i).getR()+")");
		}
		System.out.println("]");
	}
	public int searchKey(int key)
	{
		for(int i=0; i<this.p.size(); i++)
		{
			if(this.p.get(i).getL() == key)
			{
				return i;
			}
		}
		//만약 key 와 일치하는 key 가 없다면 -1 반환
		return -1;
		//searchKey end
	}
	public int searchForInsert(int key)
	{
		if(this.p.size() == 0)
			return 0;
		for(int i=0; i<this.p.size(); i++)
		{
			//삽입할 key보다 큰 key값을 최초로 발견한 위치 반환
			if(this.p.get(i).getL() > key)
			{
				return i;
			}
		}
		//삽입할 key보다 큰 key가 없는 경우 가장 끝 원소 뒤에 삽입
		return this.p.size();
	}
	public Node searchLeaf(int key)
	{
		return this;
	}
	public void printSearch(int key)
	{
		for(int i=0; i<this.p.size(); i++)
		{
			if(this.p.get(i).getL() == key)
			{
				System.out.println(p.get(i).getR());
				return;
			}
			//for end
		}
		System.out.println("NOT FOUND");
		//printSearch end
	}
	public void insert(int key, int value, Bplustree tree)
	{
		//노드가 꽉 차있는 경우
		if(p.size() == maxsize)
		{
			split(key, value, tree);
		}
		//노드가 꽉 차있지 않은 경우
		else
		{
			//새 리프 pair 생성
			PairLeaf tmp = new PairLeaf(key, value);
			//리프 pair 를 리프 노드 list 에 삽입
			//list 에 삽입 시 정렬이 유지되도록 삽입 위치 설정
			int place = searchForInsert(key);
			p.add(place, tmp);
		}
	}
	public void delete(int key, Bplustree tree)
	{
		int half = maxsize/2; //홀수일 경우 절반보다 작다.
		int dkey = this.searchKey(key);
		//delete 원하는 key 값과 일치하는 key값이 없다면
		if(dkey == -1)
		{
			System.out.println("NOT FOUND");
			return;
		}
		//delete 원하는 key값과 일치하는 key값이 있다면
		else
		{
			//leaf노드가 root인 case
			if(this.parent == null)
			{
				this.p.remove(dkey);
				return;
			}
			
			//원소의 갯수가 half인 case
			if(this.p.size() == half)
			{
				//우선 삭제
				this.p.remove(dkey);
				
				//왼쪽 형제 노드 있는 case
				if(this.l != null && this.l.parent==this.parent)
				{
					//왼쪽 borrow
					if(this.l.p.size() > half)
					{
						//left sibling의 마지막 원소를 가져온다
						PairLeaf tmp = this.l.p.remove((this.l.p.size()-1));
						this.p.add(0, tmp);
						int place = this.parent.searchForInsert(tmp.getL());
						this.parent.p.get(place).setL(tmp.getL());
						return;
					}
					//왼쪽 노드가 half인 case
					//오른쪽 형제 노드는 존재하는 case
					else if(this.r!=null && this.r.parent == this.parent)
					{
						LeafNode right = (LeafNode) this.r;
						//오른쪽 borrow
						if(right.p.size() > half)
						{
							PairLeaf tmp = right.p.remove(0);
							this.p.add(tmp);
							int place = this.parent.searchForInsert(tmp.getL()) - 1;
							this.parent.p.get(place).setL(right.p.get(0).getL());
							return;
						}
						//왼쪽노드와 merge, 
						//right sibling 있는 case 
						else
						{
							LeafNode left = this.l;
							//기존 노드의 왼소들을 왼쪽 노드로 모두 이동
							for(int i =0; i<half-1; i++)
							{
								PairLeaf tmp = this.p.remove(0);
								left.p.add(tmp);
							}
							
							//삭제될 리프노드를 제외하고 리프간 연결리스트 재구성
							left.r = right;
							right.l = left;
							
							//부모 노드에 저장된 left node의 위치를 변경시킨다.
							//부모노드에서 삭제될 pair의 위치 = place
							int place = left.parent.searchForInsert(left.p.get(0).getL());
							left.parent.p.get(place+1).setR(left);
							
							//부모노드에서 place위치의 pair를 삭제한다
							this.parent.delete(place, tree);
							return;
						}
					}
					//왼쪽노드와 merge
					//인덱스노드의 마지막 리프노드인 case(오른쪽 형제 없음)
					else
					{
						LeafNode left = this.l;
						//기존 노드의 왼소들을 왼쪽 노드로 모두 이동
						for(int i = 0; i < half-1; i++)
						{
							PairLeaf tmp = this.p.remove(0);
							left.p.add(tmp);
						}
						
						//왼쪽 노드에 해당하는 부모 노드의 키값을 변경시켜준다.
						//부모노드에서 삭제될 pair의 위치를 찾는다
						int place = left.parent.searchForInsert(left.p.get(0).getL());
						
						//왼쪽 노드 삭제 후 연결리스트 재구성
						left.l.r = this;
						this.l = left.l;
						
						//부모노드에서 place위치의 pair를 삭제한다
						left.parent.r = left;
						this.parent.delete(place, tree);
						return;
					}
				}
				//왼쪽 형제노드가 없고 오른쪽 형제노드가 있는 case
				//가장 앞쪽 노드인 경우.
				else if(this.r!=null && this.r.parent == this.parent)
				{
					LeafNode right = (LeafNode) this.r;
					//오른쪽 borrow
					if(right.p.size() > half)
					{
						PairLeaf tmp = right.p.remove(0);
						this.p.add(tmp);
						int place = this.parent.searchForInsert(right.p.get(0).getL()) - 1;
						this.parent.p.get(place).setL(right.p.get(0).getL());
						return;
					}
					//오른쪽노드와 merge
					else
					{
						//삭제될 노드의 원소들을 모두 오른쪽 노드로 이동
						for(int i =half-2; i>=0; i--)
						{
							PairLeaf tmp = this.p.remove(i);
							right.p.add(0,tmp);
						}
						//부모노드에서 삭제될 pair의 위치 얻는다
						int place = this.parent.searchForInsert(right.p.get(0).getL());
						
						//right노드의 왼쪽 형제가 사라진다
						right.l = null;
						
						//부모노드에서 place위치의 pair 삭제
						this.parent.delete(place, tree);
						return;
					}
				}
				//왼쪽 오른쪽 형제 노드 모두 없다
				//root 노드 case
				else
				{
					this.p.remove(dkey);
					return;
				}
				//원소갯수가 절반일 경우 end
			}
			//원소가 절반보다 많다면
			else if(this.p.size() > half)
			{
				this.p.remove(dkey);
				return;
			}
			//원소가 절반보다 적다면 root 노드 case
			else
			{
				this.p.remove(dkey);
				return;
			}
		}
		//delete end
	}
	public void split(int key, int value, Bplustree tree)
	{
		//PairLeaf 새로 생성
		PairLeaf tmp = new PairLeaf(key, value);
		
		//insert할 pair를 list(p)에 삽입 후 정렬한다
		//현재 해당 노드의 원소는 maxsize보다 1개 더 많다.
		int place = searchForInsert(key);
		p.add(place, tmp);
		
		//leaf node 생성
		LeafNode leaf = new LeafNode(this.maxsize);
		
		//새 leaf node 로 앞에서부터 절반까지의 원소를 이동시킨다
		//뒤의 노드의 원소가 1개 더 많거나 같다
		int half = gethalf();
		
		//원소 절반 이동시킨다
		for(int i=0; i<half; i++)
		{
			//첫번째 원소를 계속 이동시킨다(half 횟수만큼)
			PairLeaf pairleaf = p.remove(0);
			leaf.p.add(pairleaf);
		}
		//기존 노드와 leaf 노드 right 연결
		leaf.r = this;
		//왼쪽 노드가 존재하는 경우
		if(this.l != null)
		{
			//leaf 와 기존 노드의 왼쪽 노드와 right 연결
			this.l.r = leaf;
			//leaf 와 기존 노드의 왼쪽 노드와 left 연결
			leaf.l = this.l;
		}
		//기존 노드와 leaf 노드 left 연결
		this.l = leaf;
		//부모 노드로 올릴 key 값 이다.
		int upperkey = this.p.get(0).getL();
		//부모 노드가 존재하지 않는다면 새로 생성한다
		//이 경우 부모노드는 root 가 된다
		if(this.parent == null)
		{
			IndexNode index =new IndexNode(maxsize);
			//부모노드에 뒤의 노드의 가장 작은 key 와 새로운 노드(pair)를 삽입한다.
			index.insert(upperkey, leaf, tree);
			//부모노드의 rightmost child 를 기존 노드로 설정
			index.r = this;
			//기존 노드와 새로운 노드의 부모를 index 로 설정
			this.parent = index;
			leaf.parent = index;
			//새로 생성한 parent 노드 반환
			//이 경우 index 를 root 로 받아들여야 한다.
			tree.setRoot(index);
		}
		//부모 노드가 존재한다면
		else
		{
			//부모 노드가 꽉 차지 않았다면
			if(this.parent.p.size() != this.parent.maxsize)
			{
				//새 노드의 parent 를 기존 노드의 parent 로 설정
				leaf.parent = this.parent;
				//기존 parent 노드에 새로운 (키,노드)쌍을 삽입
				this.parent.insert(upperkey, leaf, tree);
			}
			//부모 노드가 꽉 차있다면
			else
			{
				IndexNode i_node = this.parent.split(upperkey, leaf, tree);
				for(int i=0; i<i_node.p.size(); i++)
				{
					i_node.p.get(i).getR().parent = i_node;
				}
				i_node.r.parent = i_node;
				//부도노드가 full인 경우 end
			}
			//부모노드가 존재하는 경우 end
		}
		//split end
	}
	//leafnode end		
}

class IndexNode extends Node
{
	public Node r;
	public List<PairIndex> p;
	
	//생성자
	public IndexNode(int degree)
	{
		super(degree);
		p = new ArrayList<PairIndex>();
		super.isLeaf = false;
	}
	//tree 순회 출력
	public void traverse()
	{
		System.out.print("[");
		for(int i=0; i<p.size(); i++)
		{
			System.out.print(p.get(i).getL() + ",");
		}
		System.out.println("]");
		for(int i=0; i<p.size(); i++)
		{
			p.get(i).getR().traverse();
		}
		r.traverse();
	}
	//leaf 노드 재귀적 검색
	public Node searchLeaf(int key)
	{	
		for(int i=0; i<p.size(); i++)
		{
			//찾는 key 보다 큰 key를 첫번째로 발견한 위치
			//해당 위치의 pair쌍의 node로 접근한다
			if(p.get(i).getL() > key)
			{
				return p.get(i).getR().searchLeaf(key);
			}
		}
		//찾는 key가 해당 노드에 존재하는 모든 key보다 큰 경우 rightmost 선택
		return this.r.searchLeaf(key);
	}
	//해당 노드의 찾는 key가 존재하는지 check
	public int searchKey(int key)
	{
		for(int i=0; i<p.size(); i++)
		{
			if(p.get(i).getL() == key)
			{
				return i;
			}
		}
		return -1;
	}
	public int searchForInsert(int key)
	{
		if(this.p.size() == 0)
			return 0;
		for(int i=0; i<this.p.size(); i++)
		{
			//삽입할 key보다 큰 key값을 최초로 발견한 위치 반환
			if(this.p.get(i).getL() > key)
			{
				return i;
			}
		}
		//삽입할 key보다 큰 key가 없는 경우 가장 마지막 원소 끝에 삽입
		return this.p.size();
	}
	//leaf 노드 찾으면서 거치는 노드 index key들 출력
	public void printSearch(int key)
	{
		for(int i=0; i<p.size(); i++)
		{
			System.out.print(p.get(i).getL() + ", ");
		}
		System.out.println("");
		int place = searchForInsert(key);
		if(place == p.size())
		{
			this.r.printSearch(key);
		}
		else
		{
			p.get(place).getR().printSearch(key);
		}
		//printSearch end
	}
	public IndexNode getleft(int position, IndexNode parent)
	{
		if(position == 0)
		{
			return null;
		}
		else
		{
			return (IndexNode) parent.p.get(position-1).getR();
		}
	}
	public IndexNode getright(int position, IndexNode parent)
	{
		if(position == parent.p.size())
		{
			return null;
		}
		else
		{
			if(position == parent.p.size()-1)
				return (IndexNode)parent.r;
			else
				return (IndexNode) parent.p.get(position+1).getR();
		}
	}
	//삽입 operation
	public void insert(int key, Node node, Bplustree tree)
	{
		// 인덱스 노드가 꽉 차있다면
		if(this.p.size() == maxsize)
		{
			split(key, node, tree);
		}
		// 인덱스 노드가 꽉 차있지 않다면
		else
		{
			//인덱스 pair 생성
			PairIndex tmp = new PairIndex(key, node);
			
			//인덱스 pair가 삽입될 위치를 찾은 후 해당 위치에 삽입.
			this.p.add(searchForInsert(key), tmp);
		}
	}
	public void delete(int place, Bplustree tree)
	{
		int half = maxsize/2;
		//인덱스 노드의 원소갯수가 절반 초과인 case
		if(this.p.size() > half)
		{
			this.p.remove(place);
			return;
		}
		//인덱스 노드의 원소갯수가 절반인 case
		else if(half == this.p.size())
		{
			//우선 삭제
			this.p.remove(place);
			
			//해당 노드가 root인 case 
			if(this.parent == null)
				return;
			
			//부모노드에서 해당 노드가 저장된 위치 = position
			int position = this.parent.searchForInsert(this.p.get(0).getL());
			IndexNode left = getleft(position, this.parent);
			IndexNode right = getright(position, this.parent);
			
			//왼쪽 노드 존재 case
			if(left != null)
			{
				//왼쪽 borrow
				if(left.p.size() > half)
				{
					//(부모 노드의 position 왼쪽키 + 왼쪽 노드의 rightmost child) pair 추가
					int movekey = this.parent.p.get(position-1).getL();
					Node movenode = left.r;
					PairIndex tmp = new PairIndex(movekey, movenode);
					this.p.add(0, tmp);
					movenode.parent = this;
					//부모 노드의 position 왼쪽 키 변경
					this.parent.p.get(position-1).setL(left.p.get(left.p.size()-1).getL());
					//왼쪽 노드의 가장 마지막 key, rightmost node 제거.
					Node leftend = left.p.get(left.p.size()-1).getR();
					left.r = leftend;
					left.p.remove(left.p.size()-1);
					return;
				}
				//왼쪽 노드 half인 경우.
				//오른쪽 노드 존재하는 case
				else if(right!=null)
				{
					//오른쪽 borrow
					if(right.p.size() > half)
					{
						//(부모 노드의 position 키 + 오른쪽 노드의 leftfirst child) pair 추가
						int movekey = this.parent.p.get(position).getL();
						Node movenode = right.p.get(0).getR();
						PairIndex tmp = new PairIndex(movekey, this.r);
						this.p.add(tmp);
						this.r = movenode;
						movenode.parent = this;
						//부모 노드의 position 키 변경
						this.parent.p.get(position).setL(right.p.get(0).getL());
						//오른쪽 노드의 첫번째 pair 제거.
						right.p.remove(0);
						return;
					}
					//왼쪽 merge
					//왼쪽 오른쪽 노드 모두 half
					else
					{
						//position-1 위치의 부모노드 키값을 가지고 온다
						int movekey = this.parent.p.get(position-1).getL();
						
						//left의 rightmost값과 move키로 새 pair 생성
						PairIndex tmp = new PairIndex(movekey, left.r);
						left.p.add(tmp);
						
						//기존 노드에서 left 노드로 원소 모두 이동
						for(int i =0; i<half-1; i++)
						{
							PairIndex temp = this.p.remove(0);
							left.p.add(temp);
							temp.getR().parent = left;
						}
						
						//기존노드의 rightmost값을 left의 rightmost로 대입.
						left.r = this.r;
						left.r.parent = left;
						
						//부모노드에서 해당 키값 제거(delete)
						left.parent.p.get(position).setR(left);
						left.parent.delete(position-1, tree);
						return;
						//왼쪽 merge end
					}
					//오른쪽 노드 존재하는 case end
				}
				//왼쪽 노드는 half, 오른쪽 노드는 없는 case
				//가장 마지막 index 노드인 case
				//왼쪽 merge
				else
				{
					//position-1 위치의 부모노드 키값을 가지고 온다
					int movekey = this.parent.p.get(position-1).getL();
					
					//left의 rightmost값과 move키로 새 pair 생성
					PairIndex tmp = new PairIndex(movekey, left.r);
					left.p.add(tmp);
					
					//기존 노드에서 left 노드로 원소 모두 이동
					for(int i =0; i<half-1; i++)
					{
						PairIndex temp = this.p.remove(0);
						left.p.add(temp);
						temp.getR().parent = left;
					}
					
					//기존노드의 rightmost값을 left의 rightmost로 대입.
					left.r = this.r;
					left.r.parent = left;
					
					//부모노드에서 해당 키값 제거(delete)
					left.parent.r = left;
					left.parent.delete(position-1, tree);
					return;
					//마지막 인덱스 노드 왼쪽 merge end
				}
				//왼쪽 노드 존재 case end
			}
			//왼쪽 노드 존재하지 않고 오른쪽 노드 존재하는 case
			//가장 앞쪽 index노드인 case
			else if(right != null)
			{
				//오른쪽 borrow
				if(right.p.size() > half)
				{
					//(부모 노드의 position 키 + 오른쪽 노드의 leftfirst child) pair 추가
					int movekey = this.parent.p.get(position).getL();
					Node movenode = right.p.get(0).getR();
					PairIndex tmp = new PairIndex(movekey, this.r);
					this.p.add(tmp);
					this.r = movenode;
					movenode.parent = this;
					//부모 노드의 position 키 변경
					this.parent.p.get(position).setL(right.p.get(0).getL());
					//오른쪽 노드의 첫번째 pair 제거.
					right.p.remove(0);
					return;
					//오른쪽 borrow end
				}
				//오른쪽 merge, 오른쪽 노드가 half인 경우
				else
				{
					//position 위치의 부모노드 키값을 가지고 온다
					int movekey = this.parent.p.get(position).getL();
					
					//현재노드의 rightmost값과 move키로 새 pair 생성, right노드에 추가
					PairIndex tmp = new PairIndex(movekey, this.r);
					right.p.add(0,tmp);
					this.r.parent = right;
					
					//기존 노드에서 right 노드로 원소 모두 이동
					for(int i =half-2; i>=0; i--)
					{
						PairIndex temp = this.p.remove(i);
						right.p.add(0,temp);
						temp.getR().parent = right;
					}
					
					//부모노드에서 해당 키값 제거(delete)
					right.parent.delete(position, tree);
					return;
					//오른쪽 merge end
				}
				//가장 처음의 인덱스 노드 case end
			}
			//원소갯수가 half개인 case end
		}
		//원소갯수가 half보다 작을 경우, root인 경우이다.
		else if(this.parent == null)
		{
			//우선 삭제한다
			this.p.remove(place);
			
			//root에 원소가 1개밖에 없을 경우 
			if(this.p.size() == 1)
			{
				//자식들이 leaf 노드인 case
				if(this.r.isLeaf)
				{
					LeafNode rightchild = (LeafNode)this.r;
					tree.setRoot(rightchild);
					return;
				}
				//자식들이 index 노드인 case
				else
				{
					IndexNode rightindex = (IndexNode)this.r;
					tree.setRoot(rightindex);
					return;
				}
				//root 원소 1개 case end
			}
			//root 원소가 1개보다 많고 half보단 적은 case
			else
				return;
			//half보다 적은 case end
		}
		//end delete
	}
	public IndexNode split(int key, Node node, Bplustree tree)
	{
		//인덱스 pair 생성
		PairIndex tmp = new PairIndex(key, node);
		
		//인덱스 노드 list 에 인덱스 pair 삽입&정렬
		//현재 maxsize보다 원소갯수가 하나 더 많다
		this.p.add(searchForInsert(key), tmp);
		
		//인덱스 노드 생성
		IndexNode index = new IndexNode(maxsize);
		int half = gethalf();
		
		//앞에서부터 half 갯수만큼의 원소들을 새 인덱스 노드로 이동
		for(int i=0; i<half; i++)
		{
			//remove first element
			PairIndex pairindex = p.remove(0);
			//put first element into new node
			index.p.add(pairindex);
		}
		//새 인덱스 노드의 rightmost child에 기존 인덱스 노드의 leftmost pair의 노드를 이동
		//기존 인덱스 노드의 leftmost pair의 키값은 부모 인덱스 노드로 올리고 해당위치에서 삭제
		index.r = this.p.get(0).getR();
		int upperkey = this.p.get(0).getL();
		this.p.remove(0);
		
		//만약 parent 인덱스가 없다면 새로 생기는 index 노드가 root가 된다
		if(this.parent == null)
		{
			//새 root 노드 생성
			IndexNode inode = new IndexNode(maxsize);
			
			//새 인덱스 pair 생성 후 새 root 노드에 삽입
			//새 root 노드의 rightmost child 에 기존 노드 삽입.
			PairIndex pindex = new PairIndex(upperkey, index);
			inode.p.add(pindex);
			inode.r = this;
			
			//앞의 노드와 뒤의 노드의 parent root 로 설정
			index.parent = inode;
			this.parent = inode;
			
			//b plus tree 의 root 의 위치를 변경해준다
			tree.setRoot(inode);
		}
		
		//만약 parent node 존재한다면
		else
		{
			//부모 노드가 꽉 차지 않았다면
			if(this.parent.p.size() != this.parent.maxsize)
			{
				//새 노드의 parent 를 기존 노드의 parent 로 설정
				//기존 parent 노드에 새로운 (새로운 키, 새 인덱스 노드)쌍을 삽입
				index.parent = this.parent;
				this.parent.insert(upperkey, index, tree);
			}
			
			//부모 노드가 꽉 차있다면
			else
			{
				IndexNode i_node = this.parent.split(upperkey, index, tree);
				for(int i=0; i<i_node.p.size(); i++)
				{
					i_node.p.get(i).getR().parent = i_node;
				}
				i_node.r.parent = i_node;
				//parent 노드 꽉 차 있다면 end
			}
			//parent 노드 존재한다면 end
		}
		//새 index 노드를 반환해주어서 parent 설정을 도와준다
		return index;
		//split end
	}
	//index class end
}

class Bplustree{
	public Node root;
	public int maxsize;
	
	public Bplustree(int degree)
	{
		root = null;
		this.maxsize = degree;
	}
	public void setRoot(LeafNode leaf)
	{
		this.root = leaf;
		this.root.parent = null;
	}
	public void setRoot(IndexNode index)
	{
		this.root = index;
		this.root.parent = null;
	}
	
	public void insertion(List<PairLeaf> list)
	{
		for(int i=0; i<list.size(); i++)
		{
			PairLeaf pair = list.get(i);
			//insertion
			if(root == null)
			{
				LeafNode leaf = new LeafNode(maxsize);
				leaf.insert(pair.getL(), pair.getR(), this);
				setRoot(leaf);
			}
			else
			{
				LeafNode tmp = (LeafNode) root.searchLeaf(pair.getL());
				tmp.insert(pair.getL(), pair.getR(), this);
			}
			//for end
		}
	}
	public void deletion(List<Integer> list)
	{
		if(root == null)
		{
			System.out.println("tree is empty!!");
			return;
		}
		for(int i=0; i<list.size(); i++)
		{
			int key = list.get(i);
			LeafNode leaf = (LeafNode)root.searchLeaf(key);
			leaf.delete(key, this);
		}
	}
	public void search(int key)
	{
		root.printSearch(key);
	}
	public void rsearch(int start, int finish)
	{
		LeafNode startLeaf = (LeafNode) root.searchLeaf(start);
		int s_point = 0;
		for(s_point=0; s_point<startLeaf.p.size(); s_point++)
		{
			if((startLeaf.p.get(s_point).getL()<=finish) && (startLeaf.p.get(s_point).getL()>=start))
				break;
			if(s_point == startLeaf.p.size() - 1)
			{
				System.out.println("NOT FOUND");
				return;
			}
		}
		LeafNode leaf = startLeaf;
		int key = leaf.p.get(s_point).getL();
		while(key <= finish)
		{
			if(s_point == leaf.p.size() - 1)
			{
				System.out.println(leaf.p.get(s_point).getL() + ", " + leaf.p.get(s_point).getR());
				if(leaf.r != null)
				{
					leaf = (LeafNode) leaf.r;
				}
				else
				{
					return;
				}
				s_point = 0;
				key = leaf.p.get(s_point).getL();
				//if end
			}
			else
			{
				System.out.println(leaf.p.get(s_point).getL() + ", " + leaf.p.get(s_point).getR());
				key = leaf.p.get(++s_point).getL();
				//else end
			}
			//while end
		}
	}
}



public class bptree {
//	private static Node root;
	
	public static void main(String[] args) {
		//make elements
		List<PairLeaf> list = new ArrayList<PairLeaf>();
		List<Integer> list2 = new ArrayList<Integer>();
		List<Integer> list3 = new ArrayList<Integer>();
		Bplustree tree = new Bplustree(10);
		for(int i=1000000; i>0; i--)
		{
			PairLeaf tmp = new PairLeaf(i, i+1000);
			list.add(tmp);
		}
		for(int i=890; i<910; i++)
		{
			list2.add(i);
		}
		for(int i=120; i<200; i++)
		{
			list3.add(i);
		}
		tree.insertion(list);
		tree.deletion(list2);
		tree.search(900);
	}
}



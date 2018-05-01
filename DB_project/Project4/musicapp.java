import java.sql.*;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Scanner;

interface MENU
{
	int EXIT=0, MANAGER=1, USER=2; 
}
class MenuViewer {
	public static Scanner keyboard = new Scanner(System.in);
	
	public static void showMenu()
	{
		System.out.println("JAEIL's MUSIC APPLICATION");
		System.out.println("0. Exit");
		System.out.println("1. Manager Menu");
		System.out.println("2. User Menu");
		System.out.print("Input: ");
	}
}
class ManagerMenu {
	public static void showMenu()
	{
		System.out.println("0. Return Upper Menu");
		System.out.println("1. Manage User");
		System.out.println("2. Manage Music");
		System.out.print("Input: ");
	}
}
class ManageUserMenu {
	public static void showMenu()
	{
		System.out.println("0. Return Upper Menu");
		System.out.println("1. Show All User");
		System.out.println("2. Insert User");
		System.out.println("3. Delete User");
		System.out.println("4. Update User Level");
		System.out.print("Input: ");
	}
}
class ManageMusicMenu {
	public static void showMenu()
	{
		System.out.println("0. Return Upper Menu");
		System.out.println("1. Show Music List");
		System.out.println("2. Insert Music");
		System.out.println("3. Delete Music");
		System.out.println("4. Update chart rank");
		System.out.print("Input: ");
	}
}
class UserMenu {
	public static void showMenu()
	{
		System.out.println("0. Return Upper Menu");
		System.out.println("1. Manage MyInfo");
		System.out.println("2. Manage Playlist");
		System.out.println("3. Search music");
		System.out.print("Input: ");
	}
}

class UserManInfoMenu {
	public static void showMenu()
	{
		System.out.println("0. Return Upper Menu");
		System.out.println("1. Change Password");
		System.out.println("2. Change Email");
		System.out.println("3. Change PhoneNumber");
		System.out.print("Input: ");
	}
}
class UserManPlayMenu {
	public static void showMenu()
	{
		System.out.println("0. Return Upper Menu");
		System.out.println("1. Show Playlist ");
		System.out.println("2. Insert Playlist ");
		System.out.println("3. Delete Playlist ");
		System.out.println("4. Delete Music From Playlist");
		System.out.println("5. Insert Music Into Playlist");
		System.out.print("Input: ");
	}
}
class DBmanager {
	String url = "jdbc:mariadb://localhost:3306/musicapp";
	String user = "root";
	String psw = "password";
	
	public void InsertMusic
	(int mno,String name,Time time,int ano,int manum)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("INSERT INTO MUSIC VALUES("+mno+",'"+name+"','"+time+"',"+
			ano+","+manum+")");
			stmt.close();
			con.close();
			System.out.println("complete insert!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void InsertUser
	(int uno,String id,String pw,int lev,String name,String email,String phone,Date date,int mno)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("INSERT INTO USER VALUES("+uno+",'"+id+"','"+pw+"',"+
			+lev+",'"+name+"','"+email+"','"+phone+"','"+date+"',"+mno+")");
			stmt.close();
			con.close();
			System.out.println("complete insert!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void InsertAlbum(int ano,String name)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("INSERT INTO ALBUM VALUES("+ano+",'"+name+"')");
			stmt.close();
			con.close();
			System.out.println("complete insert!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void InsertChartRank(int mnum,String chartname,int rank)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("INSERT INTO MAKECLIST VALUES("+mnum+",'"+chartname+"',"+rank+")");
			stmt.close();
			con.close();
			System.out.println("complete insert!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void InsertMadebySinger(int mnum,String sname,int sno)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("INSERT INTO MADEBY VALUES("+mnum+",'"+sname+"',"+sno+")");
			stmt.close();
			con.close();
			System.out.println("complete insert!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void InsertSinger(int sno,String name)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("INSERT INTO SINGER VALUES("+sno+",'"+name+"')");
			stmt.close();
			con.close();
			System.out.println("complete insert!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void InsertChart(String cname,String country)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("INSERT INTO CHART VALUES('"+cname+"','"+country+"')");
			stmt.close();
			con.close();
			System.out.println("complete insert!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void InsertGenre(int mnum,String genre)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("INSERT INTO MGENRE VALUES("+mnum+",'"+genre+"')");
			stmt.close();
			con.close();
			System.out.println("complete insert!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void InsertPlaylist(int uno,int pno,String name)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("INSERT INTO PLAYLIST VALUES("+pno+",'"+name+"',"+uno+")");
			stmt.close();
			con.close();
			System.out.println("complete insert!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void InsertMusicIntoPlay(int uno,int pno,int mno)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("INSERT INTO MAKEPLIST VALUES("+uno+","+pno+","+mno+")");
			stmt.close();
			con.close();
			System.out.println("complete insert!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	
	//END INSERT MUSIC
	public void ChangeChartRank(int mnum, String cname, int rank)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("UPDATE MAKECLIST SET RANK="+rank+" WHERE MNUM="+mnum+" AND CNM='"+cname+"'");
			stmt.close();
			con.close();
			System.out.println("complete update!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void ChangeLevel(int lev, int unum)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("UPDATE USER SET LEVEL="+lev+" WHERE UNUMBER="+unum);
			stmt.close();
			con.close();
			System.out.println("complete update!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void ChangePhone(String phone, int unum)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("UPDATE USER SET PHONE='"+phone+"' WHERE UNUMBER="+unum);
			stmt.close();
			con.close();
			System.out.println("complete update!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void ChangeEmail(String email, int unum)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("UPDATE USER SET EMAIL='"+email+"' WHERE UNUMBER="+unum);
			stmt.close();
			con.close();
			System.out.println("complete update!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void ChangePW(String pw, int unum)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("UPDATE USER SET PW='"+pw+"' WHERE UNUMBER="+unum);
			stmt.close();
			con.close();
			System.out.println("complete update!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	//END CHANGE PW
	public void DeleteUser(int del_user_num)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("DELETE FROM USER WHERE UNUMBER="+del_user_num);
			stmt.close();
			con.close();
			System.out.println("complete deleting!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	//END DELETE USER
	public void DeleteMusic(int del_music_num)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("DELETE FROM MUSIC WHERE MNUMBER="+del_music_num);
			stmt.close();
			con.close();
			System.out.println("complete deleting!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void DeleteMusicFromPlay(int uno,int pno,int mnum)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("DELETE FROM MAKEPLIST WHERE UNO="+uno+
					" AND PNO="+pno+" AND MNUM="+mnum);
			stmt.close();
			con.close();
			System.out.println("complete deleting!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	public void DeletePlaylist(int uno, int pno)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
			
			stmt.executeUpdate("DELETE FROM PLAYLIST WHERE UNO="+uno+" AND PNUMBER="+pno);
			stmt.close();
			con.close();
			System.out.println("complete deleting!!");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	//END DELETE MUSIC
	public void SearchAllUser()
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
						
			ResultSet rs;
			rs = stmt.executeQuery("SELECT * FROM USER");
			while(rs.next())
			{
				int unum = rs.getInt(1);
				String id = rs.getString(2);
				String pw = rs.getString(3);
				int lev = rs.getInt(4);
				String name = rs.getString(5);
				String email = rs.getString(6);
				String phone = rs.getString(7);
				Date date = rs.getDate(8);
				int mno = rs.getInt(9);
				System.out.println("====================");
				System.out.println("unumber: "+unum);
				System.out.println("ID: "+id);
				System.out.println("Password: "+pw);
				System.out.println("user_level: "+lev);
				System.out.println("name: "+name);
				System.out.println("email: "+email);
				System.out.println("phone: "+phone);
				System.out.println("start date: "+date);
				System.out.println("manager: "+mno);
				System.out.println("====================");
			}
			rs.close();
			stmt.close();
			con.close();
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		//end search user
	}

	public void SearchAllMusic()
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
						
			ResultSet rs;
			rs = stmt.executeQuery("SELECT * FROM MUSIC");
			while(rs.next())
			{
				int mnum = rs.getInt(1);
				String name = rs.getString(2);
				Time time = rs.getTime(3);
				int ano = rs.getInt(4);
				int manum = rs.getInt(5);
				
				System.out.println("====================");
				System.out.println("Mnumber: "+mnum);
				System.out.println("Name: "+name);
				System.out.println("PlayTime: "+time);
				System.out.println("Album Number: "+ano);
				System.out.println("Manage Number: "+manum);
				System.out.println("====================");
			}
			rs.close();
			stmt.close();
			con.close();
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		//end search user
	}
	public void SearchAllPlayer(int uno)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
						
			ResultSet rs;
			rs = stmt.executeQuery("SELECT * FROM PLAYLIST WHERE UNO ="+uno);
			while(rs.next())
			{
				int pnum = rs.getInt(1);
				String name = rs.getString(2);
				int unum = rs.getInt(3);
				
				System.out.println("====================");
				System.out.println("Pnumber: "+pnum);
				System.out.println("Name: "+name);
				System.out.println("User Number: "+unum);
				System.out.println("====================");
			}
			rs.close();
			stmt.close();
			con.close();
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		//end 
	}
	public void NmSearchMusic(String musicName)
	{
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
						
			ResultSet rs;
			rs = stmt.executeQuery("SELECT * FROM MUSIC WHERE NAME="+"'"+musicName+"'");
			while(rs.next())
			{
				int mnum = rs.getInt(1);
				String name = rs.getString(2);
				Time time = rs.getTime(3);
				int ano = rs.getInt(4);
				int manum = rs.getInt(5);
				
				System.out.println("====================");
				System.out.println("Mnumber: "+mnum);
				System.out.println("Name: "+name);
				System.out.println("PlayTime: "+time);
				System.out.println("Album Number: "+ano);
				System.out.println("Manage Number: "+manum);
				System.out.println("====================");
			}
			rs.close();
			stmt.close();
			con.close();
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		//end search music with name
	}
	public int CheckUser(String id, String pw)
	{
		int unum;
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
						
			ResultSet rs;
			rs = stmt.executeQuery("SELECT UNUMBER FROM USER WHERE ID='"+id+"' AND PW='"+pw+"'");
			if(rs.next())
			{
				unum = rs.getInt(1);
				rs.close();
				stmt.close();
				con.close();
				return unum;
			}
			rs.close();
			stmt.close();
			con.close();
			
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		//end check user ID, PW
		return -1;
	}
	public int CheckManager(int mnum, String pw)
	{
		int unum;
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
						
			ResultSet rs;
			rs = stmt.executeQuery("SELECT MNUMBER FROM MANAGER WHERE MNUMBER="+mnum+" AND PW='"+pw+"'");
			if(rs.next())
			{
				unum = rs.getInt(1);
				rs.close();
				stmt.close();
				con.close();
				return unum;
			}
			rs.close();
			stmt.close();
			con.close();
			
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		//end check manage ID, PW
		return -1;
	}
	public int CheckLevel(int uno)
	{
		int level;
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
						
			ResultSet rs;
			rs = stmt.executeQuery("SELECT LEVEL FROM USER WHERE UNUMBER="+uno);
			if(rs.next())
			{
				level = rs.getInt(1);
				rs.close();
				stmt.close();
				con.close();
				return level;
			}
			rs.close();
			stmt.close();
			con.close();
			
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		//end check level
		return -1;
	}
	public int CheckAlbum(int ano)
	{
		int anumber;
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
						
			ResultSet rs;
			rs = stmt.executeQuery("SELECT ANUMBER FROM ALBUM WHERE ANUMBER="+ano);
			if(rs.next())
			{
				anumber = rs.getInt(1);
				rs.close();
				stmt.close();
				con.close();
				return anumber;
			}
			rs.close();
			stmt.close();
			con.close();
			
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		//end check level
		return -1;
	}
	public int CheckSinger(String snm, int sno)
	{
		int anumber;
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
						
			ResultSet rs;
			rs = stmt.executeQuery("SELECT SNUMBER FROM SINGER WHERE SNUMBER="+sno+" AND "
					+ "SNAME='"+snm+"'");
			if(rs.next())
			{
				anumber = rs.getInt(1);
				rs.close();
				stmt.close();
				con.close();
				return anumber;
			}
			rs.close();
			stmt.close();
			con.close();
			
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		//end check level
		return -1;
	}
	public int CheckChart(String cname)
	{
		int anumber;
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
						
			ResultSet rs;
			rs = stmt.executeQuery("SELECT CNAME FROM CHART WHERE CNAME='"+cname+"'");
			if(rs.next())
			{
				
				rs.close();
				stmt.close();
				con.close();
				return 1;
			}
			rs.close();
			stmt.close();
			con.close();
			
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		//end check level
		return -1;
	}
	public int CheckMusic(int mnum)
	{
		int anumber;
		try
		{
			Connection con = DriverManager.getConnection(url,user,psw);
			Statement stmt = con.createStatement();
						
			ResultSet rs;
			rs = stmt.executeQuery("SELECT MNUMBER FROM MUSIC WHERE MNUMBER="+mnum);
			if(rs.next())
			{
				
				rs.close();
				stmt.close();
				con.close();
				return 1;
			}
			rs.close();
			stmt.close();
			con.close();
			
		} catch(Exception e)
		{
			e.printStackTrace();
		}
		//end check level
		return -1;
	}
	//end dbmanager
}

class managerAction {
	int m_choice;
	int c;
	String str,aname,sname,mname,genre;
	int tmp,rank;
	int uno,ano,lev,sno,mus_no;
	String id,pw,name,email,phone,country;
	Date date;
	Time time;
	DBmanager dbm = new DBmanager();
	
	
	public managerAction(int mno)
	{
		while(true)
		{
			ManagerMenu.showMenu();
			m_choice = MenuViewer.keyboard.nextInt();
			switch(m_choice)
			{
			case 0:
				return;
			case 1:
				manUser(mno);
				break;
			case 2:
				manMusic(mno);
				break;
			default:
				break;					
			}
		}
	}
	public void manUser(int mno)
	{
		while(true)
		{
			ManageUserMenu.showMenu();
			c=MenuViewer.keyboard.nextInt();
			switch(c)
			{
			case 0:
				return;
			case 1:
				dbm.SearchAllUser();
				break;
			case 2:
				System.out.print("input unumber: ");
				uno = MenuViewer.keyboard.nextInt();
				System.out.print("input level: ");
				lev = MenuViewer.keyboard.nextInt();
				System.out.print("input id: ");
				MenuViewer.keyboard.nextLine();
				id = MenuViewer.keyboard.nextLine();
				System.out.print("input pw: ");
				pw = MenuViewer.keyboard.nextLine();
				System.out.print("input name: ");
				name = MenuViewer.keyboard.nextLine();
				System.out.print("input email: ");
				email = MenuViewer.keyboard.nextLine();
				System.out.print("input phone(010xxxxyyyy): ");
				phone = MenuViewer.keyboard.nextLine();
				System.out.print("input start date(yyyy-mm-dd): ");
				str = MenuViewer.keyboard.nextLine();
				date = java.sql.Date.valueOf(str);
				dbm.InsertUser(uno, id, pw, lev, name, email, phone, date, mno);
				break;
			case 3:
				System.out.print("input user number: ");
				tmp = MenuViewer.keyboard.nextInt();
				dbm.DeleteUser(tmp);
				break;
			case 4:
				System.out.print("input level and user number: ");
				lev = MenuViewer.keyboard.nextInt();
				tmp = MenuViewer.keyboard.nextInt();
				dbm.ChangeLevel(lev, tmp);
				break;
			}
			//end while
		}
	}
	public void manMusic(int mno)
	{
		System.out.println("manMusic");
		while(true)
		{
			ManageMusicMenu.showMenu();
			c=MenuViewer.keyboard.nextInt();
			switch(c)
			{
			case 0:
				return;
			case 1:
				dbm.SearchAllMusic();
				break;
			case 2:
				System.out.print("input album number: ");
				ano = MenuViewer.keyboard.nextInt();
				if(dbm.CheckAlbum(ano)==-1)
				{
					System.out.println("create new album");
					System.out.print("input album name: ");
					MenuViewer.keyboard.nextLine();
					aname = MenuViewer.keyboard.nextLine();
					dbm.InsertAlbum(ano, aname);
				}
				System.out.print("input singer number:");
				sno = MenuViewer.keyboard.nextInt();
				System.out.print("input singer name:");
				MenuViewer.keyboard.nextLine();
				sname = MenuViewer.keyboard.nextLine();
				if(dbm.CheckSinger(sname, sno)==-1)
				{
					System.out.println("create new singer");
					dbm.InsertSinger(sno, sname);
				}
				System.out.print("input music number: ");
				mus_no = MenuViewer.keyboard.nextInt();
				System.out.print("input music name: ");
				MenuViewer.keyboard.nextLine();
				mname=MenuViewer.keyboard.nextLine();
				System.out.print("input Play Time(mm:ss:00): ");
				str = MenuViewer.keyboard.nextLine();
				time = java.sql.Time.valueOf(str);
				dbm.InsertMusic(mus_no, mname, time, ano, mno);
				System.out.print("input genre: ");
				genre = MenuViewer.keyboard.nextLine();
				dbm.InsertGenre(mus_no, genre);
				dbm.InsertMadebySinger(mus_no, sname, sno);
				System.out.println("finishing insert music^^");
				
				break;
			case 3:
				System.out.print("input music number: ");
				tmp = MenuViewer.keyboard.nextInt();
				dbm.DeleteMusic(tmp);
				break;
			case 4:
				System.out.print("input music number: ");
				tmp = MenuViewer.keyboard.nextInt();
				if(dbm.CheckMusic(tmp)==-1)
				{
					System.out.println("there is no music of which number is "+tmp);
					break;
				}
				System.out.print("input chart name: ");
				MenuViewer.keyboard.nextLine();
				str = MenuViewer.keyboard.nextLine();
				if(dbm.CheckChart(str)==-1)
				{
					System.out.println("insert new chart");
					System.out.print("input country which chart belong to: ");
					country = MenuViewer.keyboard.nextLine();
					dbm.InsertChart(str, country);
					System.out.print("input chart rank: ");
					rank = MenuViewer.keyboard.nextInt();
					dbm.InsertChartRank(rank, str, tmp);
					break;
				}
				else
				{
					System.out.println("update chart rank");
					System.out.print("input chart rank: ");
					rank = MenuViewer.keyboard.nextInt();
					dbm.ChangeChartRank(tmp, str, rank);
					break;
				}
			}
			//end while
		}
	}
}

class userAction {
	int u_choice;
	int c;
	int tmp,mno,pno;
	String str;
	DBmanager dbm = new DBmanager();
	public userAction(int uno)
	{
		while(true)
		{
			UserMenu.showMenu();
			u_choice = MenuViewer.keyboard.nextInt();
			switch(u_choice)
			{
			case 0:
				return;
			case 1:
				changeMyInfo(uno);
				break;
			case 2:
				changePlaylist(uno);
				break;
			case 3:
				searchMusic();
				break;
			default:
				break;					
			}
		}
	}
	public void changeMyInfo(int uno)
	{
		while(true)
		{
			UserManInfoMenu.showMenu();
			c = MenuViewer.keyboard.nextInt();
			switch(c)
			{
			case 0:
				return;
			case 1:
				System.out.print("Input new password: ");
				MenuViewer.keyboard.nextLine();
				str = MenuViewer.keyboard.nextLine();
				dbm.ChangePW(str, uno);
				System.out.println("complete update!");
				break;
			case 2:
				System.out.print("Input new email: ");
				MenuViewer.keyboard.nextLine();
				str = MenuViewer.keyboard.nextLine();
				dbm.ChangeEmail(str, uno);
				System.out.println("complete update!");
				break;
			case 3:
				System.out.print("Input new phone number(don't write bar): ");
				MenuViewer.keyboard.nextLine();
				str = MenuViewer.keyboard.nextLine();
				dbm.ChangePhone(str, uno);
				System.out.println("complete update!");
				break;
			default :
				break;
			}
		}
	}
	public void changePlaylist(int uno)
	{
		while(true)
		{
			UserManPlayMenu.showMenu();
			c = MenuViewer.keyboard.nextInt();
			switch(c)
			{
			case 0:
				return;
			case 1:
				dbm.SearchAllPlayer(uno);
				break;
			case 2:
				System.out.print("input playlist number: ");
				tmp = MenuViewer.keyboard.nextInt();
				System.out.print("input playlist name: ");
				MenuViewer.keyboard.nextLine();
				str = MenuViewer.keyboard.nextLine();
				dbm.InsertPlaylist(uno, tmp, str);
				break;
			case 3:
				System.out.print("input playlist number: ");
				tmp = MenuViewer.keyboard.nextInt();
				dbm.DeletePlaylist(uno, tmp);
				break;
			case 4:
				System.out.print("input playlist number and music number: ");
				pno = MenuViewer.keyboard.nextInt();
				mno = MenuViewer.keyboard.nextInt();
				dbm.DeleteMusicFromPlay(uno, pno, mno);
				break;
			case 5:
				tmp = dbm.CheckLevel(uno);
				if(tmp != 1)
				{
					System.out.println("Sorry.. you are not level 1");
					break;
				}
				else
				{
					System.out.println("your level is 1, welcome^^");
					System.out.print("input music number and playlist number: ");
					mno = MenuViewer.keyboard.nextInt();
					pno = MenuViewer.keyboard.nextInt();
					dbm.InsertMusicIntoPlay(uno, pno, mno);
					break;
				}
			}
		}
	}
	public void searchMusic()
	{
		System.out.print("input music name: ");
		MenuViewer.keyboard.nextLine();
		str = MenuViewer.keyboard.nextLine();
		dbm.NmSearchMusic(str);
	}
}

public class musicapp {

	public static void main(String[] args) {
		
		int choice;
		int uno;
		int mno;
		String id;
		String pw;
		try
		{
			DBmanager dbm = new DBmanager();
			while(true)
			{
				MenuViewer.showMenu();
				choice = MenuViewer.keyboard.nextInt();
				
				switch(choice)
				{
				case MENU.EXIT :
					System.out.println("See you Later^^..");
					return;
				case MENU.MANAGER :
					System.out.print("insert Manager Number: ");
					mno = MenuViewer.keyboard.nextInt();
					System.out.print("insert pw: ");
					MenuViewer.keyboard.nextLine();
					pw = MenuViewer.keyboard.nextLine();
					/////////////////
					mno = dbm.CheckManager(mno, pw);
					if(mno==-1)
					{
						System.out.println("try again..");
						break;
					}
					else
					{
						managerAction manAction = new managerAction(mno);
						break;
					}
				case MENU.USER :
					System.out.print("insert id: ");
					MenuViewer.keyboard.nextLine();
					id = MenuViewer.keyboard.nextLine();
					System.out.print("insert pw: ");
					pw = MenuViewer.keyboard.nextLine();
					/////////////////
					uno = dbm.CheckUser(id, pw);
					if(uno==-1)
					{
						System.out.println("try again..");
						break;
					}
					else
					{
						userAction userAct = new userAction(uno);
						break;
					}
				}
			}//end while
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
////end main
	}
}

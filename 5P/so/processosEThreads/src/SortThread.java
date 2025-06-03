import java.util.ArrayList;

public class SortThread extends Thread {

  private ArrayList<Integer> array;

  public SortThread(ArrayList<Integer> array) {
    this.array = array;
    this.start();
  }

  @Override
  public void run() {
    ArrayList<Integer> sortedArray = new ArrayList<>(array);
    sortedArray.sort(null);
  }

}

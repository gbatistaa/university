import java.util.ArrayList;

public class SortThread extends Thread {
  private ArrayList<Integer> array;

  public SortThread(ArrayList<Integer> array) {
    this.array = array;
  }

  @Override
  public void run() {
    ArrayList<Integer> sortedArray = new ArrayList<>(array);
    sortedArray.sort(null);
    StringBuilder buffer = new StringBuilder("[");
    for (int i = 0; i < sortedArray.size(); i++) {
      buffer.append(sortedArray.get(i));
      if (i < sortedArray.size() - 1)
        buffer.append(", ");
    }
    buffer.append("]");
    System.out.println("Sorted: " + buffer);
  }
}

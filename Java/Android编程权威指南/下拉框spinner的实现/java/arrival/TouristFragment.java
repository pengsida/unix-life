package strp.arrival;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

/**
 * Created by pengsida on 2017/4/1.
 */

public class TouristFragment extends Fragment
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstancesState)
    {
        // 创建视图层的对象
        View v = inflater.inflate(R.layout.fragment_tourist, container, false);
        return v;
    }
}

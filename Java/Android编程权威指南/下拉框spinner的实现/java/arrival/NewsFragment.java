package strp.arrival;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;


/**
 * Created by pengsida on 2017/4/1.
 */

public class NewsFragment extends Fragment
{
    @Override
    public void onCreate(Bundle savedInstancesState)
    {
        // 创建模型层的对象
        super.onCreate(savedInstancesState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstancesState)
    {
        // 创建视图层的对象
        View v = inflater.inflate(R.layout.fragment_news, container, false);
        return v;
    }
}

package strp.arrival;

import android.content.Context;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.TranslateAnimation;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {
    private static final String IDENTITY = "IDENTITY";

    private int identity;
    Fragment fragmentNews;
    Fragment fragmentIdentity;

//    private final int NEWS = 0;
//    private final int FUNC = 1;
//    private int pageIndex;
//
//    public final int BEFORELOGIN = 0;
//    public final int GUIDE = 1;
//    public final int TOURIST = 2;
//    public int state = BEFORELOGIN;
//
//    private final int LOGIN_RET = 0;
//
//    private ViewPager viewPager;
//    private ArrayList<View> viewList;
//    private TextView btnNewsView;
//    private TextView btnFuncView;
//
//    // 滚动条图片
////    private ImageView scrollbar;
//    // 滚动条初始偏移量
//    private int offset = 0;
//    // 滚动条宽度
////    private int bmpW;
//    //一倍滚动量
//    private int one;
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_main);
//
//        identity = getIntent().getBooleanExtra(IDENTITY, true);
//
//        // 添加想要切换的界面
//        LayoutInflater inflater = getLayoutInflater();
//        View view_news = inflater.inflate(R.layout.view_news, null);
//        View view_guide_func = inflater.inflate(R.layout.view_guide_func, null);
//        View view_tourist_func = inflater.inflate(R.layout.view_tourist_func, null);
//
//        viewList = new ArrayList<>();
//        viewList.add(view_news);
//        if (identity)
//            viewList.add(view_tourist_func);
//        else
//            viewList.add(view_guide_func);
//
//        viewPager = (ViewPager) findViewById(R.id.viewPager);
//
//        //数据适配器
//        PagerAdapter pagerAdapter = new PagerAdapter(){
//
//            @Override
//            public int getCount() {
//                return viewList.size();
//            }
//
//            @Override
//            public boolean isViewFromObject(View view, Object object) {
//                return view == object;
//            }
//
//            @Override
//            //使从ViewGroup中移出当前View
//            public void destroyItem(ViewGroup container, int position, Object object) {
//                container.removeView(viewList.get(position));
//            }
//
//            @Override
//            //返回一个对象，这个对象表明了PagerAdapter适配器选择哪个对象放在当前的ViewPager中
//            public Object instantiateItem(ViewGroup container, int position) {
//                container.addView(viewList.get(position));
//                return viewList.get(position);
//            }
//        };
//
//        viewPager.setAdapter(pagerAdapter);
//        viewPager.setCurrentItem(NEWS);
//        viewPager.addOnPageChangeListener(new MyOnPageChangeListener());
//
//        // 得到屏幕宽度
//        DisplayMetrics displayMetrics = new DisplayMetrics();
//        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
//        int screenW = displayMetrics.widthPixels;
//
////        scrollbar = (ImageView)findViewById(R.id.scrollbar);
//        // 获取滚动条的宽度
//        // BitmapFactory.decodeResource(getResources(), R.drawable.scrollbar).setWidth(screenW/2);
////        bmpW = BitmapFactory.decodeResource(getResources(), R.drawable.scrollbar).getWidth();
//        //计算出滚动条初始的偏移量
////        offset = (screenW / 2 - bmpW) / 2;
//        //计算出切换一个界面时，滚动条的位移量
////        one = offset * 2 + bmpW;
////        Matrix matrix = new Matrix();
////        matrix.postTranslate(offset, 0);
//        //将滚动条的初始位置设置成与左边界间隔一个offset
////        scrollbar.setImageMatrix(matrix);
//
//        // 设置页面切换按钮
//        btnNewsView = (TextView) findViewById(R.id.newsView);
//        btnNewsView.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                viewPager.setCurrentItem(NEWS);
//            }
//        });
//        btnFuncView = (TextView) findViewById(R.id.funcView);
//        btnFuncView.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                viewPager.setCurrentItem(FUNC);
//            }
//        });
//
//        // 设置登录按钮
////        Button login_btn = (Button) findViewById(R.id.login);
////        login_btn.setOnClickListener(new View.OnClickListener() {
////            @Override
////            public void onClick(View v) {
////                Intent intent = new Intent();
////                intent.setClass(MainActivity.this, LoginActivity.class);
////                startActivityForResult(intent, LOGIN_RET);
////            }
////        });
//
//        // 设置导游功能按钮
//        Button guide_msg_btn = (Button) view_guide_func.findViewById(R.id.guide_msg_entry);
//        guide_msg_btn.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent();
//                intent.setClass(MainActivity.this, GuideMsgActivity.class);
//                startActivity(intent);
//            }
//        });
//        Button guide_guide_btn = (Button) view_guide_func.findViewById(R.id.guide_guide_entry);
//        guide_guide_btn.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent();
//                intent.setClass(MainActivity.this, GuideGuideActivity.class);
//                startActivity(intent);
//            }
//        });
//        Button guide_map_btn = (Button) view_guide_func.findViewById(R.id.guide_map_entry);
//        guide_map_btn.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent();
//                intent.setClass(MainActivity.this, GuideMapActivity.class);
//                startActivity(intent);
//            }
//        });
//        Button guide_info_btn = (Button) view_guide_func.findViewById(R.id.guide_info_entry);
//        guide_info_btn.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent();
//                intent.setClass(MainActivity.this, GuideInfoActivity.class);
//                startActivity(intent);
//            }
//        });
//        Button guide_room_btn = (Button) view_guide_func.findViewById(R.id.guide_room_entry);
//        guide_room_btn.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent();
//                intent.setClass(MainActivity.this, GuideRoomActivity.class);
//                startActivity(intent);
//            }
//        });
//        Button guide_name_btn = (Button) view_guide_func.findViewById(R.id.guide_name_entry);
//        guide_name_btn.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent();
//                intent.setClass(MainActivity.this, GuideNameActivity.class);
//                startActivity(intent);
//            }
//        });
//
//        // 设置游客功能按钮
//        Button tourist_msg_btn = (Button) view_tourist_func.findViewById(R.id.tourist_msg_entry);
//        tourist_msg_btn.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent();
//                intent.setClass(MainActivity.this, TouristMsgActivity.class);
//                startActivity(intent);
//            }
//        });
//        Button tourist_guide_btn = (Button) view_tourist_func.findViewById(R.id.tourist_guide_entry);
//        tourist_guide_btn.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent();
//                intent.setClass(MainActivity.this, TouristGuideActivity.class);
//                startActivity(intent);
//            }
//        });
//        Button tourist_map_btn = (Button) view_tourist_func.findViewById(R.id.tourist_map_entry);
//        tourist_map_btn.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent();
//                intent.setClass(MainActivity.this, TouristMapActivity.class);
//                startActivity(intent);
//            }
//        });
//        Button tourist_info_btn = (Button) view_tourist_func.findViewById(R.id.tourist_info_entry);
//        tourist_info_btn.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent();
//                intent.setClass(MainActivity.this, TouristInfoActivity.class);
//                startActivity(intent);
//            }
//        });
//    }
//
//    @Override
//    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
//        super.onActivityResult(requestCode, resultCode, data);
//        switch (requestCode) {
//            case LOGIN_RET:
//                if (resultCode == RESULT_OK) {
//
//                }
//                break;
//            default:
//                break;
//        }
//    }
//
//    public class MyOnPageChangeListener implements ViewPager.OnPageChangeListener {
//        @Override
//        public void onPageSelected(int position) {
//            Animation animation = null;
//            switch (position) {
//                case NEWS:
//                    /**
//                     * TranslateAnimation的四个属性分别为
//                     * float fromXDelta 动画开始的点离当前View X坐标上的差值
//                     * float toXDelta 动画结束的点离当前View X坐标上的差值
//                     * float fromYDelta 动画开始的点离当前View Y坐标上的差值
//                     * float toYDelta 动画开始的点离当前View Y坐标上的差值
//                     **/
//                    animation = new TranslateAnimation(one, 0, 0, 0);
//                    break;
//                case FUNC:
//                    animation = new TranslateAnimation(offset, one, 0, 0);
//                    break;
//            }
//            //arg0为切换到的页的编码
//            pageIndex = position;
//            // 将此属性设置为true可以使得图片停在动画结束时的位置
//            animation.setFillAfter(true);
//            //动画持续时间，单位为毫秒
//            animation.setDuration(200);
//            //滚动条开始动画
////            scrollbar.startAnimation(animation);
//        }
//
//        @Override
//        public void onPageScrolled(int arg0, float arg1, int arg2) {
//        }
//
//        @Override
//        public void onPageScrollStateChanged(int arg0) {
//        }
//    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main1);

        final FragmentManager fragmentManager = getSupportFragmentManager();
        fragmentNews = new NewsFragment();
        identity = getIntent().getIntExtra(IDENTITY, 0);
        TextView newsTextView = (TextView)findViewById(R.id.newsView);
        TextView funcTextView = (TextView)findViewById(R.id.funcView);

        fragmentManager.beginTransaction().add(R.id.fragment_container, fragmentNews).commit();

        if (identity == 1)
            fragmentIdentity = new TouristFragment();
        else if(identity == 2)
            fragmentIdentity = new GuideFragment();
        else
        {
            funcTextView.setText("注册");
            fragmentIdentity = new RegisterFragment();
        }

        newsTextView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                fragmentManager.beginTransaction().replace(R.id.fragment_container, fragmentNews).commit();
            }
        });

        funcTextView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                fragmentManager.beginTransaction().replace(R.id.fragment_container, fragmentIdentity).commit();
            }
        });
    }

    public static Intent newIntent(Context context, int identity)
    {
        Intent i = new Intent(context, MainActivity.class);
        i.putExtra(IDENTITY, identity);
        return i;
    }
}

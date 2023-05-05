<!-- login.vue -->
<template>
  <view class="container">
    <image class="logo" src="/static/logo.png" />
    <form class="form">
      <view class="input-box">
        <input type="text" placeholder="用户名" v-model="username" class="input" />
      </view>
      <view class="input-box">
        <input type="password" placeholder="密码" v-model="password" class="input" />
      </view>
      <button @click="login" class="button">登录</button>
    </form>
  </view>
</template>

<script>
export default {
  data() {
    return {
      username: '',
      password: '',
      isConfirmWindowShown: false
    };
  },
  mounted() {
    window.addEventListener('popstate', this.showConfirmWindow);
  },
  methods: {
    login() {
      // 判断用户名和密码是否正确
      if (this.username === '1' && this.password === '1') {
        // 登录成功后将登录状态保存到本地缓存中
        uni.setStorageSync('isLogin', true);
        // 登录成功后跳转到指定页面
        uni.navigateTo({
          url: '/pages/index/index'
        });
      } else {
        uni.showToast({
          title: '用户名或密码错误',
          icon: 'none'
        });
      }
    },
    showConfirmWindow() {
      if (!this.isConfirmWindowShown) {
        this.isConfirmWindowShown = true;
        const that = this;
        uni.showModal({
          title: '提示',
          content: '您是否想要离开该页面？',
          success: function(res) {
            if (res.confirm) {
              uni.reLaunch({
                url: '/pages/index/login'
              });
            } else {
				uni.navigateTo({
				  url: '/pages/index/index'
				});
              that.isConfirmWindowShown = false;
              history.pushState(null, null, location.href);
            }
          },
          complete: function() {
            that.isConfirmWindowShown = false;
          }
        });
      } else {
        history.pushState(null, null, location.href);
      }
    }
  },
  beforeDestroy() {
    window.removeEventListener('popstate', this.showConfirmWindow);
  }
};
</script>

<style>
.container {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100vh;
  background-color: #f5f5f5;
}

.form {
  width: 80%;
  max-width: 400px;
  background-color: #ffffff;
  border-radius: 10px;
  box-shadow: 0px 2px 10px rgba(0, 0, 0, 0.1);
  padding: 20px;
}

.input-box {
  margin-bottom: 20px;
}

.input {
  width: 100%;
  height: 40px;
  border: none;
  border-radius: 5px;
  padding: 10px;
  font-size: 16px;
  background-color: #f5f5f5;
}

.button {
  display: block;
  width: 100%;
  height: 40px;
  border: none;
  border-radius: 5px;
  background-color: #7f7f7f;
  color: #ffffff;
  font-size: 16px;
  cursor: pointer;
}
</style>

<style>
.container {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  height: 70vh;
  background-color: #ffffff;
}

/* .logo {
  width: 100%;
  max-width: 200px;
  margin-bottom: 20px;
} */
.logo {
  max-width: 120px;
  max-height: 140px;
}

.form {
  width: 80%;
  max-width: 400px;
  background-color: #ffffff;
  border-radius: 20px;
  box-shadow: 0px 2px 10px rgba(0, 0, 0, 0.1);
  padding: 20px;
}

.input-box {
  margin-bottom: 20px;
}

.input {
  width: 93%;
  height: 40px;
  border: none;
  border-radius: 5px;
  padding: 10px;
  font-size: 16px;
  background-color: #f5f5f5;
}

.button {
  display: block;
  width: 100%;
  height: 40px;
  border: none;
  border-radius: 5px;
  background-color: #7f7f7f;
  color: #ffffff;
  font-size: 16px;
  cursor: pointer;
}

.button:active {
  background-color: #266262;
}



</style>

<template>
<div id="setting-content">
  <div id="setting-content-container">

  <v-form :items="form1" class="form-item"></v-form>
  <v-form :items="form2" class="form-item"></v-form>
  <v-form :items="form3" class="form-item"></v-form>

  </div>
</div>
</template>

<script>
import vForm from './Form'

export default {
  components: {
    vForm
  },
  props: [ 'user' ],
  data() {
    return {
      form1: [
        { title: '游戏ID', text: this.user.id },
        { title: '我的等级', text: this.user.level },
        { title: '昵称', text: this.user.name }
      ],
      form2: [
        { title: '性别', text: this.user.gender,
          callback: this.openPopup('gender', this)
        },
        { title: '地区', text: this.user.province },
        { title: '个性签名', text: this.user.sign.substring(0, 5) + '...' }
      ],
      form3: [
        { title: '手机', text: this.user.phone || '未绑定' }
      ]
    }
  },
  methods: {
    openPopup(name, self) {
      return function() {
        console.log(name, self);
        self.$store.commit('setPopupStatus', { name: name, value: true })
      }
    }
  }
}
</script>

<style>
#setting-content {
  position: relative;
  width: 100%;
  height: 82vh;
}
#setting-content-container {
  position: relative;
  width: 78%;
  max-height: 95%;
  height: auto;
  margin: 0 auto;
  overflow-y: auto;
  overflow-x: none;
  padding: 4%;
}
.form-item {
  margin-bottom: 5vh;
}

</style>
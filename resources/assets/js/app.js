
/**
 * First we will load all of this project's JavaScript dependencies which
 * includes Vue and other libraries. It is a great starting point when
 * building robust, powerful web applications using Vue and Laravel.
 */

require('./bootstrap');

window.$ = window.jQuery = require('jquery');
window.Vue = require('vue');

/* Ctrl + S押下時のデフォルト動作を抑制 */
$(function() {
    $(window).keydown(function(e) {
        if ((e.which == '115' || e.which == '83' ) && (e.ctrlKey || e.metaKey)) {
            e.preventDefault();
            return false;
        }
        return true;
    });
});

/**
 * Next, we will create a fresh Vue application instance and attach it to
 * the page. Then, you may begin adding components to this application
 * or customize the JavaScript scaffolding to fit your unique needs.
 */

Vue.component('example-component', require('./components/ExampleComponent.vue'));
Vue.component('new-proj-form', require('./components/main-panel/NewProjFormComponent.vue'));
Vue.component('side-nav', require('./components/main-panel/SideNavComponent.vue'));
Vue.component('toggle-button', require('./components/main-panel/ToggleButtonComponent.vue'));
Vue.component('main-panel', require('./components/main-panel/MainPanelComponent.vue'));

const app = new Vue({
    el: '#app'
});

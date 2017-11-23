<template>
    <li>
        <div>
            <form action="create" method="POST" id="new-proj-form">
                <input name="_token" :value="token" type="hidden" v-if="token">
                <label>
                    <h2>
                        <input type="text"
                            name="proj_name"
                            value=""
                            id="new-proj-name"
                            placeholder="新しいプロジェクトの名前"
                            @blur="submit"
                            @keypress.esc="cancel"
                            required
                        >
                    </h2>
                </label>
            </form>
        </div>
    </li>
</template>

<script>
    export default {
        props: {
            token: String
        },
        data() {
            return {
                proj_name: 'untitled'
            };
        },
        mounted() {
            $(this.$el).find('#new-proj-name').focus();
        },
        methods: {
            submit() {
                let $form = $('#new-proj-form');
                if ($form[0].checkValidity()) {
                    $form.submit();
                } else {
                    this.cancel();
                }
            },
            cancel() {
                this.$emit('cancel');
            }
        }
    }
</script>
